#include "qsimDetector.hh"
#include "G4SDManager.hh"
#include "qsimDetectorConstruction.hh"
#include "G4RunManager.hh"

qsimDetector::qsimDetector( G4String name, G4int detnum ) : G4VSensitiveDetector(name){

    fDetNo = detnum;
    assert( fDetNo > 0 );

    fHCID = -1;
    fTrackSecondaries = true;
    
    G4String colname = "genhit_" + name + "_" + std::to_string(detnum);
    collectionName.insert(colname);
}

qsimDetector::~qsimDetector(){
}

void qsimDetector::Initialize(G4HCofThisEvent *){
    fHitColl = new qsimDetectorHitsCollection( SensitiveDetectorName, collectionName[0] );
}


G4bool qsimDetector::ProcessHits( G4Step *step, G4TouchableHistory *){

    // Get touchable volume info
    G4TouchableHistory *hist = (G4TouchableHistory*)(step->GetPreStepPoint()->GetTouchable());
    G4int copyID = hist->GetReplicaNumber();

    G4StepPoint *prestep = step->GetPreStepPoint();
    G4StepPoint *poststep = step->GetPostStepPoint();
    G4Track     *track   = step->GetTrack();

    //printf("Standard detector %d hit by %s!\n", fDetNo, track->GetParticleDefinition()->GetParticleName().data());

    //  Make pointer to new hit if it's a valid track
    qsimDetectorHit *thishit;

    G4bool isPostStepOutsideSD; //Checks if the reflected light is counted as a hit 
    isPostStepOutsideSD = poststep->GetPhysicalVolume()->GetLogicalVolume()->GetSensitiveDetector() != nullptr;

    // check if the physics volume is the sensitive detector
    if(isPostStepOutsideSD){
        thishit = new qsimDetectorHit(fDetNo, copyID);
        fHitColl->insert( thishit );

        // Hit
        thishit->f3X = prestep->GetPosition();
        thishit->f3V = track->GetVertexPosition();
        thishit->f3D = track->GetVertexMomentumDirection();
        thishit->f3P = track->GetMomentum();

        thishit->fP = track->GetMomentum().mag();
        thishit->fE = track->GetTotalEnergy();
        thishit->fM = track->GetDefinition()->GetPDGMass();

        thishit->fTrID  = track->GetTrackID();
        thishit->fmTrID = track->GetParentID();
        thishit->fPID   = track->GetDefinition()->GetPDGEncoding();

        // FIXME - Enumerate encodings
        thishit->fGen   = (long int) track->GetCreatorProcess();
    }
    return true;
}

void qsimDetector::EndOfEvent(G4HCofThisEvent*HCE) {
    G4SDManager *sdman = G4SDManager::GetSDMpointer();

    if(fHCID<0){ fHCID = sdman->GetCollectionID(collectionName[0]); }

    HCE->AddHitsCollection( fHCID, fHitColl );
    return;
}


