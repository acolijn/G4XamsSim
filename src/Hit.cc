#include "Hit.hh"
#include "G4SystemOfUnits.hh"

/**
 * @namespace G4Sim
 * @brief Namespace for the G4Sim library.
/*/
namespace G4Sim {

G4ThreadLocal G4Allocator<Hit>* HitAllocator = 0;

/**
 * @brief Default constructor for the Hit class.
 */
Hit::Hit()
    : G4VHit(), energyDeposit(0.), position(G4ThreeVector()), time(0.), trackID(-1), parentID(-1), momentum(G4ThreeVector()), particleType(""), processType(""), used(false) {}

Hit::~Hit() {}

/**
 * @brief Prints the information of the Hit object.
 *
 * This function prints the ID, parent ID, process type, particle type, particle energy,
 * energy deposit, position, and time of the Hit object. The values are printed in appropriate units.
 */
void Hit::Print() const {
    G4cout << "ID: " << trackID
           << ", ID: " << parentID
           << ", Proc: " << processType
           << ", Ptcl: " << particleType
           << ", E0: " << particleEnergy0 / keV << " keV"
           << ", E1: " << particleEnergy1 / keV << " keV"
           << ", dE: " << energyDeposit / keV << " keV"
           << ", pos: " << position / cm << " cm"
           << ", t: " << time / ns << " ns"
           << G4endl;
    //       << ", p: " << momentum << G4endl;
}

} // namespace G4Sim