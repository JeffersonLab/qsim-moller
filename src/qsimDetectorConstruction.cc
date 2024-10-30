#include "qsimDetectorConstruction.hh"
#include "G4SystemOfUnits.hh"
#include "qsimDetector.hh"
#include "qsimScintDetector.hh"
#include "G4SDManager.hh"
#include "G4LogicalVolume.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "G4GDMLParser.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

qsimDetectorConstruction::qsimDetectorConstruction(const G4GDMLParser& parser): G4VUserDetectorConstruction(), fParser(parser){
  
    G4SDManager* SDman = G4SDManager::GetSDMpointer();
    qsimDetector* cathSD = new qsimDetector("PhotoCathode",1);
    SDman->AddNewDetector(cathSD);

    // Retrieve Auxiliary Information for sensitive detector
    const G4GDMLAuxMapType* auxmap = fParser.GetAuxMap();
    G4cout << G4endl << "Found " << auxmap->size()
        << " volume(s) with auxiliary information." << G4endl;

    // Loop over the auxiliary information, attach it to the volumes and print it
    for(G4GDMLAuxMapType::const_iterator iter=auxmap->begin(); iter!=auxmap->end(); iter++){
        G4cout << "Volume " << ((*iter).first)->GetName()
            << " has the following list of auxiliary information: " << G4endl ;
        for (G4GDMLAuxListType::const_iterator vit=(*iter).second.begin(); vit!=(*iter).second.end(); vit++){
            G4cout << "--> Type: " << (*vit).type
            << ", Value: " << (*vit).value << G4endl;
        }

        for (G4GDMLAuxListType::const_iterator vit=(*iter).second.begin(); vit!=(*iter).second.end();vit++){
            // Sensitive detector (SensDet) = name
            if ((*vit).type=="SensDet" && (*vit).value=="PhotoCathode"){
                G4cout << "Attaching sensitive detector " << (*vit).value
                    << " to volume " << ((*iter).first)->GetName()
                    <<  G4endl;
            
                G4VSensitiveDetector* mydet = SDman->FindSensitiveDetector((*vit).value);
                
                if (mydet){
                    G4LogicalVolume* myvol = (*iter).first;
                    myvol->SetSensitiveDetector(mydet);
                } else{
                    G4cout << (*vit).value << " detector not found" << G4endl;
                }
            }

            // Color = name
            if ((*vit).type == "Color") {
                G4Colour colour(1.0,1.0,1.0);
                if (G4Colour::GetColour((*vit).value, colour)) {

                    G4cout << "Setting color to " << (*vit).value << "." << G4endl;

                    G4VisAttributes visAttribute(colour);
                    ((*iter).first)->SetVisAttributes(visAttribute);

                } else {
                G4cout << "Colour " << (*vit).value << " is not known." << G4endl;
                }
            }

            // Alpha = float between 0 and 1
            if ((*vit).type == "Alpha") {
                G4Colour colour(1.0,1.0,1.0);
                const G4VisAttributes* visAttribute_old = ((*iter).first)->GetVisAttributes();

                if (visAttribute_old != nullptr)
                colour = visAttribute_old->GetColour();

                G4Colour colour_new(
                    colour.GetRed(), 
                    colour.GetGreen(),
                    colour.GetBlue(),
                    std::atof((*vit).value.c_str()));
                G4VisAttributes visAttribute_new(colour_new);
                ((*iter).first)->SetVisAttributes(visAttribute_new);
            }
        }
        G4cout << G4endl;
    }
}

qsimDetectorConstruction::~qsimDetectorConstruction(){;}

G4VPhysicalVolume* qsimDetectorConstruction::Construct() {
  return fParser.GetWorldVolume();
}