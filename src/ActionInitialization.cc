#include "G4RunManager.hh"
#include "DetectorConstruction.hh"	
#include "ActionInitialization.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "EventAction.hh"
#include "SteppingAction.hh"

/**
 * @namespace G4Sim
 *
 * @brief Namespace for the G4Sim simulation.
 *
 * This namespace contains the classes and functions related to the G4Sim simulation.
/*/
namespace G4Sim
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

/**
 * @class ActionInitialization
 * @brief Initializes user actions for the simulation.
 *
 * This class is responsible for initializing user-defined actions for the simulation.
 * It inherits from G4VUserActionInitialization and provides a constructor to initialize the base class.
 */
ActionInitialization::ActionInitialization()
  : G4VUserActionInitialization()
{
}

/**
 * @brief Builds the actions for the master thread.
 *
 * This function is responsible for initializing the actions for the master thread.
 * It creates a new instance of the EventAction class and sets it as the user action
 * for the RunAction. This ensures that the appropriate actions are executed when
 * running the simulation on the master thread.
 */
void ActionInitialization::BuildForMaster() const
{
  auto eventAction = new EventAction;
  SetUserAction(new RunAction(eventAction));
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

/**
 * @brief Builds the action initialization for the simulation.
 *
 * This function sets up the user actions for the simulation, including the primary generator action,
 * event action, run action, and stepping action. It creates instances of these actions and sets them
 * as user actions using the SetUserAction() function.
 */
void ActionInitialization::Build() const
{
  SetUserAction(new PrimaryGeneratorAction);

  auto eventAction = new EventAction();
  SetUserAction(eventAction);

  auto runAction = new RunAction(eventAction);
  SetUserAction(runAction);

  auto steppingAction = new SteppingAction(eventAction);
  SetUserAction(steppingAction);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

} // namespace G4FastSim
