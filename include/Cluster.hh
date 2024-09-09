#ifndef CLUSTER_HH
#define CLUSTER_HH

#include <vector>
#include "G4ThreeVector.hh"
#include "Hit.hh"

/**
 * @namespace G4Sim
 * @brief Namespace for the G4Sim library.
/*/
namespace G4Sim {

struct Cluster {
    G4ThreeVector position;
    G4double energyDeposit;
    G4double time;
    std::vector<Hit*> hits;
};

} // namespace G4FastSim

#endif
