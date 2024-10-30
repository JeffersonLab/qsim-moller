#ifndef qsimDetectorConstruction_h
#define qsimDetectorConstruction_h 

#include "globals.hh"
#include "G4VUserDetectorConstruction.hh"

class G4GDMLParser;

class qsimDetectorConstruction : public G4VUserDetectorConstruction{
    public:
        qsimDetectorConstruction(const G4GDMLParser& parser);
        ~qsimDetectorConstruction();

    public:
        G4VPhysicalVolume* Construct();

    private:
        const G4GDMLParser& fParser;

    public:
        G4double fDetAngle, fQuartzPolish;
        G4double fDetPosX, fDetPosY;
};

#endif /*qsimDetectorConstruction_h*/
