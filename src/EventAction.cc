#include "EventAction.hh"
#include "EventActionMessenger.hh"	
#include "RunAction.hh"

#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4AnalysisManager.hh"
#include "G4EventManager.hh"
#include "G4TransportationManager.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4Navigator.hh"
#include "G4SDManager.hh"
#include "Hit.hh"
#include "G4SystemOfUnits.hh"

/**
 * @namespace G4Sim
 * @brief Namespace for the G4Sim library.
/*/
namespace G4Sim {

//std::mutex EventAction::mtx;

EventAction::EventAction() : G4UserEventAction(),
      fSpatialThreshold(2.5 * cm),  // Default values
      fTimeThreshold(5.0 * ns)
{
  // set printing per each event
  G4RunManager::GetRunManager()->SetPrintProgress(1);

  fMessenger = new EventActionMessenger(this);
}

EventAction::~EventAction() {
  delete fMessenger;
}


/**
 * @brief Adds a hits collection name to the EventAction.
 * 
 * This function adds the specified hits collection name to the EventAction's list of hits collection names.
 * 
 * @param name The name of the hits collection to be added.
 */
void EventAction::AddHitsCollectionName(const G4String& name) {
    G4cout << "EventAction::AddHitsCollectionName: " << name << G4endl;
    fHitsCollectionNames.push_back(name);
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

/**
 * @brief This function is called at the beginning of each event.
 * 
 * It resets variables and retrieves information about the primary vertex of the event.
 * 
 * @param event Pointer to the G4Event object representing the current event.
 */
void EventAction::BeginOfEventAction(const G4Event* event)
{
  if (verbosityLevel > 0)
    G4cout << "EventAction::BeginOfEventAction..... NEXT" << G4endl;	

  // Reset variables
  ResetVariables();

  //G4cout<<"EventAction::BeginOfEventAction next event...."<<G4endl;
  G4PrimaryVertex* primaryVertex = event->GetPrimaryVertex();
  fXp = primaryVertex->GetPosition().x();
  fYp = primaryVertex->GetPosition().y();
  fZp = primaryVertex->GetPosition().z();

  //auto def =  event->GetPrimaryVertex()->GetPrimary()->GetParticleDefinition();
  //G4cout << " def = " << def->GetParticleName() << G4endl;
  if (verbosityLevel > 0){
    G4cout << "EventAction::BeginOfEventAction Primary vertex: x = "<< primaryVertex->GetPosition() / cm << " (cm)"<<G4endl;
    G4cout << "                                                p = "<< primaryVertex->GetPrimary()->GetMomentumDirection() << G4endl;
    G4cout << "                                                E = "<< primaryVertex->GetPrimary()->GetKineticEnergy() / keV << " keV"<< G4endl;
  }
}

/**
 * @brief Resets the variables of the EventAction class.
 *
 * This function sets all the member variables of the EventAction class to their initial values.
 * - fEdep: Energy deposition (in MeV).
 * - fLogWeight: Logarithm of the weight.
 * - fEventType: Event type.
 * - fNclusters: Number of clusters.
 * - fNphot: Number of photons.
 * - fNcomp: Number of components.
 * - fXp: X position of the primary particle.
 * - fYp: Y position of the primary particle.
 * - fZp: Z position of the primary particle.
 * - fEd: Vector of energy depositions.
 * - fX: Vector of X positions.
 * - fY: Vector of Y positions.
 * - fZ: Vector of Z positions.
 * - fW: Vector of weights.
 */
void EventAction::ResetVariables() {
  fLogWeight = 0.0;
  fEventType = 0;
  fXp = 0.0;
  fYp = 0.0;
  fZp = 0.0;

  // cluster information
  fE.clear();
  fX.clear();
  fY.clear();
  fZ.clear();
  fW.clear();
  fID.clear();
  // detector information
  fEdet.clear();
  fNdet.clear();
  fNphot.clear();
  fNcomp.clear();

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

/**
 * @brief This function is called at the end of each event in the simulation.
 * It analyzes the hits and clusters, fills the analysis manager's ntuple with relevant data,
 * and updates the event ID and other variables.
 * 
 * @param event Pointer to the current G4Event object.
 */
void EventAction::EndOfEventAction(const G4Event* event)
{
  if(verbosityLevel>0) G4cout << "EventAction::EndOfEventAction..... Analyze hits and cluster...." << G4endl;
  AnalyzeHits(event);

  const G4Event* currentEvent = G4EventManager::GetEventManager()->GetConstCurrentEvent();
  fEventID = currentEvent->GetEventID();
  
  if(verbosityLevel<0) G4cout << "EventAction::EndOfEventAction..... Fill ntuple...." << G4endl;
  // Get analysis manager
  auto analysisManager = G4AnalysisManager::Instance();
  //std::lock_guard<std::mutex> lock(mtx);
  
  // get the energy depositis in keV
  analysisManager->FillNtupleDColumn(0, 0, fEventID);
  analysisManager->FillNtupleDColumn(0, 1, fLogWeight);
  analysisManager->FillNtupleDColumn(0, 2, fEventType);
  analysisManager->FillNtupleDColumn(0, 3, fXp);
  analysisManager->FillNtupleDColumn(0, 4, fYp);
  analysisManager->FillNtupleDColumn(0, 5, fZp);

  analysisManager->AddNtupleRow(0);

  if(verbosityLevel>0) G4cout << "EventAction::EndOfEventAction: Done...." << G4endl;	

}

G4double EventAction::CalculateDistance(const G4ThreeVector& pos1, const G4ThreeVector& pos2) {
    return (pos1 - pos2).mag();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4double EventAction::CalculateTimeDifference(G4double time1, G4double time2) {
    return std::fabs(time1 - time2);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

/**
 * @brief Analyzes the hits in the given event and clusters them based on spatial and time thresholds.
 * 
 * This function retrieves the hits collections from the event, processes each collection to extract hits,
 * and then clusters these hits using predefined spatial and time thresholds. The resulting clusters are
 * stored in a vector for further analysis or output.
 * 
 * @param event Pointer to the G4Event object containing the hits to be analyzed.
 * 
 * The function performs the following steps:
 * 1. Retrieves the hits collections from the event.
 * 2. Iterates over each hits collection specified in `fHitsCollectionNames`.
 * 3. For each collection, retrieves the hits and stores them in a temporary vector.
 * 4. Retrieves the spatial and time thresholds for clustering from a configuration file or predefined map.
 * 5. Clusters the hits based on the retrieved thresholds.
 * 6. Merges the clusters from all collections into a single vector `allClusters`.
 * 
 * If no hits collection is found for the event, a warning is issued.
 */
void EventAction::AnalyzeHits(const G4Event* event) {
    G4HCofThisEvent* HCE = event->GetHCofThisEvent();
    if (!HCE) {
        G4ExceptionDescription msg;
        msg << "No hits collection of this event found." << G4endl;
        G4Exception("EventAction::EndOfEventAction()", "MyCode0001", JustWarning, msg);
        return;
    }

    std::vector<Cluster> allClusters;

    // Loop over hits collections.
    for (size_t i = 0; i < fHitsCollectionNames.size(); ++i) {
        G4int hcID = G4SDManager::GetSDMpointer()->GetCollectionID(fHitsCollectionNames[i]);
        auto* fHitsCollection = static_cast<HitsCollection*>(HCE->GetHC(hcID));
        if (!fHitsCollection) continue;

        G4int n_hit = fHitsCollection->entries();
        if (verbosityLevel > 0) G4cout << "Hits Collection: " << fHitsCollectionNames[i] << " has " << n_hit << " hits." << G4endl;

        std::vector<Hit*> collectionHits;
        for (G4int j = 0; j < n_hit; ++j) {
            Hit* hit = (*fHitsCollection)[j];
            collectionHits.push_back(hit);
        }

        // Get clustering parameters for this collection from the config file or a predefined map.
        G4double spatialThreshold = GetSpatialThreshold(fHitsCollectionNames[i]) * mm;
        G4double timeThreshold = GetTimeThreshold(fHitsCollectionNames[i]) * ns;

        // Cluster hits for this collection.
        std::vector<Cluster> clusters;
        ClusterHits(collectionHits, spatialThreshold, timeThreshold, clusters, static_cast<int>(i)); // Add collection ID here.
        allClusters.insert(allClusters.end(), clusters.begin(), clusters.end());
    
        // Use `allClusters` for further analysis or output.
        G4double edet = 0.0;
        G4int nclus = 0;
        G4int ncomp = 0;
        G4int nphot = 0;

        for (auto& cluster : clusters) {

          if (cluster.energyDeposit > 0*eV) {
            nclus++;
            edet += cluster.energyDeposit / keV;
            //G4cout << "cluster: " << nclus << " edep: " << cluster.energyDeposit / keV << " keV" << G4endl;
            fE.push_back(cluster.energyDeposit / keV);
            fX.push_back(cluster.position.x());
            fY.push_back(cluster.position.y());
            fZ.push_back(cluster.position.z());
            fID.push_back(cluster.collectionID);
            fW.push_back(fLogWeight);
          }
        }
        //G4cout << "Total energy deposit: " << edet << " keV" << G4endl;
        fEdet.push_back(edet);
        fNdet.push_back(nclus);
        fNphot.push_back(ncomp);
        fNcomp.push_back(nphot);
    }
  }

/**
 * @brief Clusters hits based on spatial and temporal thresholds.
 *
 * This function processes a list of hits, normalizes their times relative to the start of the event,
 * and clusters them based on spatial and temporal thresholds. It first identifies cluster seeds based
 * on the process type (e.g., Compton or photoelectric), then clusters the remaining hits, and finally
 * merges clusters that are close together.
 *
 * @param hits A vector of pointers to Hit objects to be clustered.
 * @param spatialThreshold The maximum spatial distance between hits to be considered part of the same cluster.
 * @param timeThreshold The maximum time difference between hits to be considered part of the same cluster.
 * @param clusters A vector of Cluster objects where the resulting clusters will be stored.
 * @param collectionID An identifier for the collection of hits being processed.
 */
void EventAction::ClusterHits(std::vector<Hit*>& hits, G4double spatialThreshold, G4double timeThreshold, std::vector<Cluster>& clusters, int collectionID) {

    if (hits.empty()) return;  // No hits, nothing to do.

    // Find the earliest hit time to normalize times relative to the start of the event.
    G4double startTime = hits[0]->time;
    for (const auto& hit : hits) {
        if (hit->time < startTime) {
            startTime = hit->time;
        }
    }

    // Normalize hit times to the start of the event.
    for (auto& hit : hits) {
        hit->time -= startTime;
    }

    // Find cluster seeds based on the process (e.g., Compton or photoelectric).
    G4int ncomp = 0;
    G4int nphot = 0;

    for (auto& hit : hits) {
        G4String process = hit->processType;
        G4int trackID = hit->trackID;

        G4bool isRelevantProcess = (process == "compt" || process == "phot");

        if (process == "compt") ncomp++;
        if (process == "phot") nphot++;
        if (isRelevantProcess) {
            clusters.push_back(Cluster{hit->position, hit->energyDeposit, hit->time, {hit}, collectionID});
            hit->used = true;
        }
    }

    // Cluster the remaining hits.
    for (auto& hit : hits) {
        if (hit->used) continue;

        bool addedToCluster = false;
        for (auto& cluster : clusters) {
            if (cluster.collectionID != collectionID) continue;  // Ensure we only cluster within the same collection.

            if (CalculateDistance(hit->position, cluster.position) < spatialThreshold &&
                CalculateTimeDifference(hit->time, cluster.time) < timeThreshold) {

                G4double energyDeposit = hit->energyDeposit;
                if (energyDeposit > 0 * eV) {
                    G4int clusterSize = cluster.hits.size();
                    cluster.position = (cluster.position * clusterSize + hit->position) / (clusterSize + 1);
                    cluster.energyDeposit += energyDeposit;
                    cluster.time = (cluster.time * clusterSize + hit->time) / (clusterSize + 1);
                    cluster.hits.push_back(hit);
                    addedToCluster = true;
                    break;
                }
            }
        }
        if (!addedToCluster) {
            clusters.push_back(Cluster{hit->position, hit->energyDeposit, hit->time, {hit}, collectionID});
        }
    }

    // Merge clusters that are close together.
    for (size_t i = 0; i < clusters.size(); ++i) {
        for (size_t j = i + 1; j < clusters.size();) {
            if (clusters[i].collectionID != clusters[j].collectionID) {
                ++j;
                continue;
            }
            if (CalculateDistance(clusters[i].position, clusters[j].position) < spatialThreshold &&
                CalculateTimeDifference(clusters[i].time, clusters[j].time) < timeThreshold) {

                // Merge cluster j into cluster i.
                G4int totalHits = clusters[i].hits.size() + clusters[j].hits.size();
                clusters[i].position = (clusters[i].position * clusters[i].hits.size() + clusters[j].position * clusters[j].hits.size()) / totalHits;
                clusters[i].energyDeposit += clusters[j].energyDeposit;
                clusters[i].time = (clusters[i].time * clusters[i].hits.size() + clusters[j].time * clusters[j].hits.size()) / totalHits;
                clusters[i].hits.insert(clusters[i].hits.end(), clusters[j].hits.begin(), clusters[j].hits.end());

                clusters.erase(clusters.begin() + j);
            } else {
                ++j;
            }
        }
    }
}

G4double EventAction::GetSpatialThreshold(const G4String& collectionName) {
    if (fClusteringParameters.find(collectionName) != fClusteringParameters.end()) {
        return fClusteringParameters[collectionName].first;
    }
    return 10.0 * mm;  // default value if not found
}

G4double EventAction::GetTimeThreshold(const G4String& collectionName) {
    if (fClusteringParameters.find(collectionName) != fClusteringParameters.end()) {
        return fClusteringParameters[collectionName].second;
    }
    return 100.0 * ns;  // default value if not found
}

std::map<G4String, std::pair<G4double, G4double>> EventAction::fClusteringParameters;

void EventAction::SetClusteringParameters(const std::map<G4String, std::pair<G4double, G4double>>& params) {
    fClusteringParameters = params;
}


} // namespace G4Sim