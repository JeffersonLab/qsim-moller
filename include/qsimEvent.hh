#ifndef __QSIMEVENT_HH
#define __QSIMEVENT_HH

/*!
   Event information class.  This needs to
   contain all the information we need to
   generate particles and provide sufficient
   output.
*/

#include <vector>
#include "G4ThreeVector.hh"

class G4ParticleDefinition;

class qsimEvent {
    public:
		qsimEvent();
		~qsimEvent();

		void ProduceNewParticle( G4ThreeVector, G4ThreeVector, G4String, G4double); // with particle name
		void ProduceNewParticle( G4ThreeVector, G4ThreeVector, G4int, G4double); // with particle ID
		void Reset();
		void UndoLastParticle();

		G4bool EventIsSane();
		void   Print();

    public:
		// Particles to be produced
		std::vector<G4ThreeVector>    fPartPos;
		std::vector<G4ThreeVector>    fPartMom;  // Generated direction (no ms)
		std::vector<G4ParticleDefinition *> fPartType;
		std::vector<G4double>         fPartRate;
};

#endif//__QSIMEVENT_HH
