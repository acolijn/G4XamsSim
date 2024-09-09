#ifndef EventAction_h
#define EventAction_h 1

#include "EventActionMessenger.hh"
#include "G4UserEventAction.hh"
#include "G4ThreeVector.hh"	
#include "G4String.hh"
#include "G4Types.hh"
#include "Cluster.hh"
#include "globals.hh"

#include <vector>
#include <mutex>

///
/// Event action class
///

class G4Event;
class G4HCofThisEvent;
class G4VHitsCollection;

/**
 * @namespace G4Sim
 * @brief Namespace for the G4Sim library.
/*/
namespace G4Sim
{

// Use enum to define the constants
enum EventType {
    DIRECT_GAMMA = 0,
    SCATTERED_GAMMA = 1
};

/**
 * @class EventAction
 * @brief Event action class for handling events in the simulation.
 *
 * This class inherits from G4UserEventAction and provides methods for handling the beginning and end of events,
 * analyzing hits, and resetting variables. It also provides getter and setter methods for accessing event data.
 * The class includes functions for hit clustering and calculating distances and time differences.
 *
 * The variables stored for each event in the ntuple tree include the logarithm of the weight, energy deposition,
 * number of clusters, number of photons, number of components, event ID, event type, and position coordinates.
 * The class also includes vectors for storing energy deposition, position coordinates, and weights.
 *
 * The class uses a mutex for thread safety and includes a messenger for setting event action properties.
 */
class EventAction : public G4UserEventAction
{
  public:
    EventAction();
    ~EventAction();

    void BeginOfEventAction(const G4Event* event);// override;
    void EndOfEventAction(const G4Event* event);// override;

    std::vector<G4double>& GetX(){return fX;};
    std::vector<G4double>& GetY(){return fY;};
    std::vector<G4double>& GetZ(){return fZ;};
    std::vector<G4double>& GetE(){return fEd;};
    std::vector<G4double>& GetW(){return fW;};

    void AddEdep(G4double edep) { fEdep += edep; }
    void AnalyzeHits(const G4Event* event);
    void ResetVariables();

    void SetSpatialThreshold(G4double value) { fSpatialThreshold = value; }
    void SetTimeThreshold(G4double value) { fTimeThreshold = value; }
    void AddHitsCollectionName(const G4String& name);

  private:

    G4double fSpatialThreshold;  // Spatial threshold for clustering
    G4double fTimeThreshold;     // Time threshold for clustering

    //
    // functions for hit clustering
    //
    void ClusterHits(std::vector<G4Sim::Hit*>& hits, G4double spatialThreshold, G4double timeThreshold, std::vector<Cluster>& clusters);

    G4double CalculateDistance(const G4ThreeVector& pos1, const G4ThreeVector& pos2);
    G4double CalculateTimeDifference(G4double time1, G4double time2);

    // define here all the variables that you want to store for each event in the 
    // ntuple tree  
    G4double fLogWeight;
    G4double fEdep;
    G4int fNclusters;
    G4int fNphot;
    G4int fNcomp;
    G4int fEventID;
    G4int fEventType;
    G4double fXp;
    G4double fYp;
    G4double fZp;

    std::vector<G4double> fEd;
    std::vector<G4double> fX;
    std::vector<G4double> fY;
    std::vector<G4double> fZ;
    std::vector<G4double> fW;


    //static std::mutex mtx; // Mutex for thread safety
    std::vector<G4String> fHitsCollectionNames;
    G4int verbosityLevel=0;

  protected:
    EventActionMessenger* fMessenger;
};

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif


