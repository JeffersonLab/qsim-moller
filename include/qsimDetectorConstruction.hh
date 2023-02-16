#ifndef qsimDetectorConstruction_h
#define qsimDetectorConstruction_h 

#include "globals.hh"
#include "G4VUserDetectorConstruction.hh"

class G4GDMLParser;

class qsimDetectorConstruction : public G4VUserDetectorConstruction{
    public:
        qsimDetectorConstruction(const G4GDMLParser& parser);
        ~qsimDetectorConstruction();
        //void StandModeSet();
		void DetModeSet(G4int );
		void QModeSet(G4int );
		void StandModeSet(G4int );

    public:
        G4VPhysicalVolume* Construct();

    private:
        const G4GDMLParser& fParser;
        G4int fDetMode;
        G4int fQMode;
        G4int fStandMode;
        

    public:
        G4double fDetAngle, fQuartzPolish;
        G4double fDetPosX, fDetPosY;
};

#endif /*qsimDetectorConstruction_h*/
