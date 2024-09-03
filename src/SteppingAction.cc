#include "SteppingAction.hh"
#include "EventAction.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4LogicalVolume.hh"
#include "G4SystemOfUnits.hh"
#include "G4SDManager.hh"
#include "G4VProcess.hh"
#include "G4ProcessType.hh"

namespace G4XamsSim
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SteppingAction::SteppingAction(EventAction* eventAction): G4UserSteppingAction(){

}


SteppingAction::~SteppingAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
/**
 * @brief Performs the user-defined stepping action for each step of a particle in the simulation.
 *
 * This function is called for each step of a particle in the simulation. It checks if the event is a fast simulation event,
 * and if not, it calls the `AnalyzeStandardStep` function to analyze the standard step. If the event is a fast simulation event,
 * it performs various operations based on the particle's properties and the current volume.
 *
 * @param step The G4Step object representing the current step of the particle.
 */
void SteppingAction::UserSteppingAction(const G4Step* step)
{
  if (verbosityLevel >= 2) Print(step);
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

/**
 * Prints information about the given step.
 *
 * @param step The step for which to print information.
 */
void SteppingAction::Print(const G4Step* step) {

  G4Track* track = step->GetTrack();
  G4LogicalVolume* volume_pre = step->GetPreStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume();
  G4String processType = step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName();

  // if Verbose level >=2 print this information

  G4cout << "SteppingAction::Print track ID             : " << track->GetTrackID() << G4endl;
  G4cout << "                      track parent ID      : " << track->GetParentID() << G4endl;
  G4cout << "                      particle name        : " << track->GetParticleDefinition()->GetParticleName() << G4endl;
  G4cout << "                      energy               : " << track->GetKineticEnergy() / keV << " keV"<< G4endl;
  G4cout << "                      volume_pre name      : " << volume_pre->GetName() << G4endl;
  G4cout << "                      volume_pre position  : " << step->GetPreStepPoint()->GetPosition()/cm << " cm"<<G4endl;
  G4cout << "                      volume_post position : " << step->GetPostStepPoint()->GetPosition()/cm << " cm"<<G4endl;
  G4cout << "                      process type         : " << processType << G4endl;
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

} // namespace G4FastSim
