#ifndef qsimIO_HH
#define qsimIO_HH

#include "TROOT.h"
#include "TObject.h"
#include "G4Run.hh"
#include "qsimtypes.hh"
#include "G4String.hh"
#include "qsimEvent.hh"

class TFile;
class TTree;
class qsimDetectorHit;
class qsimScintDetectorHit;

#define __IO_MAXHIT 20000
#define __FILENAMELEN 255

// Units for output
#define __E_UNIT GeV
#define __L_UNIT m
#define __T_UNIT ns
#define __ANG_UNIT rad
#define __ASYMM_SCALE 1e-9 // ppb
#define __RATE_UNIT hertz

class qsimIO {
    public:
		qsimIO();
		~qsimIO();

		void SetFilename(G4String  fn);
		G4String GetFilename(){return fFilename;}
		void FillTree();
		void Flush();
		void WriteTree();
		void WriteRun();
		void InitializeTree();

    private:
		TFile *fFile;
		TTree *fTree;
	
		char fFilename[__FILENAMELEN];

		//  Interfaces and buffers to the tree
		//  This is potentially going to get very long...

		// Event data
    public:
		void SetEventData(qsimEvent *);
    private:
		Double_t fEvPart_X;
		Double_t fEvPart_Y;
		Double_t fEvPart_Z;
		Double_t fEvPart_P;
		Double_t fEvPart_Px;
		Double_t fEvPart_Py;
		Double_t fEvPart_Pz;
		Double_t fEvPart_Rate;
		Int_t fEvPart_PID;

	//  DetectorHit
    public:
		void AddDetectorHit(qsimDetectorHit *);
    private:
		Int_t fNDetHit;
		Int_t fDetHit_det[__IO_MAXHIT];
		Int_t fDetHit_id[__IO_MAXHIT];

		Int_t fDetHit_trid[__IO_MAXHIT];
		Int_t fDetHit_pid[__IO_MAXHIT];
		Int_t fDetHit_gen[__IO_MAXHIT];
		Int_t fDetHit_mtrid[__IO_MAXHIT];

		Double_t fDetHit_X[__IO_MAXHIT];
		Double_t fDetHit_Y[__IO_MAXHIT];
		Double_t fDetHit_Z[__IO_MAXHIT];

		Double_t fDetHit_Px[__IO_MAXHIT];
		Double_t fDetHit_Py[__IO_MAXHIT];
		Double_t fDetHit_Pz[__IO_MAXHIT];
		Double_t fDetHit_P[__IO_MAXHIT];
		Double_t fDetHit_E[__IO_MAXHIT];
		Double_t fDetHit_M[__IO_MAXHIT];

		Double_t fDetHit_Vx[__IO_MAXHIT];
		Double_t fDetHit_Vy[__IO_MAXHIT];
		Double_t fDetHit_Vz[__IO_MAXHIT];
		Double_t fDetHit_Vdx[__IO_MAXHIT];
		Double_t fDetHit_Vdy[__IO_MAXHIT];
		Double_t fDetHit_Vdz[__IO_MAXHIT];

	//  ScintDetectorHit
    public:
		void AddScintDetectorHit(qsimScintDetectorHit *);
    private:
		Int_t fNScintDetHit;
		Int_t fScintDetHit_det[__IO_MAXHIT];
		Int_t fScintDetHit_id[__IO_MAXHIT];

		Double_t fScintDetHit_edep[__IO_MAXHIT];

};

#endif//qsimIO_HH
