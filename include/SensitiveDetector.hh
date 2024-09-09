#ifndef SENSITIVEDETECTOR_HH
#define SENSITIVEDETECTOR_HH

#include "G4VSensitiveDetector.hh"
#include "G4THitsCollection.hh"
#include "Hit.hh" // Include the Hit class

/**
 * @namespace G4Sim
 * @brief Namespace for the G4Sim library.
/*/
namespace G4Sim {

/**
 * @class SensitiveDetector
 * 
 * @brief This class represents a sensitive detector in the simulation.
 * 
 * The SensitiveDetector class is derived from the G4VSensitiveDetector class and is responsible for handling hits and energy deposits in the simulation.
 * It provides methods for initializing the detector, processing hits, and ending the event.
 * The class also includes a method to retrieve the total energy deposit.
 * 
 * @note This class assumes the existence of a HitsCollection class and a Hit class.
 */
class SensitiveDetector : public G4VSensitiveDetector {
public:
    SensitiveDetector(const G4String& name, const G4String& hitsCollectionName);
    virtual ~SensitiveDetector();

    virtual void Initialize(G4HCofThisEvent* hce) override;
    virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory* history) override;
    virtual void EndOfEvent(G4HCofThisEvent* hce) override;

    G4double GetTotalEnergyDeposit() const { return fTotalEnergyDeposit; }

private:
    HitsCollection* fHitsCollection;
    //G4THitsCollection<Hit>* fHitsCollection;
    G4double fTotalEnergyDeposit;
    G4int fHitsCollectionID;
};

} // namespace G4Sim

#endif
