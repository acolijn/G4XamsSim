#include "G4RunManager.hh"
#include "DetectorConstruction.hh"	
#include "ActionInitialization.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "EventAction.hh"
#include "SteppingAction.hh"

namespace G4Sim
{

ActionInitialization::ActionInitialization()
  : G4VUserActionInitialization()
{
}

void ActionInitialization::BuildForMaster() const
{
  auto eventAction = new EventAction;
  SetUserAction(new RunAction(eventAction));
}

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

} // namespace G4FastSim
