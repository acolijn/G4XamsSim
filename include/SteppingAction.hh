#ifndef SteppingAction_h
#define SteppingAction_h 1

#include "G4UserSteppingAction.hh"
#include "G4ParticleTable.hh"
#include "globals.hh"

class G4LogicalVolume;

/// Stepping action class
///

/**
 * @namespace G4Sim
 * @brief Namespace for the G4Sim library.
/*/
namespace G4Sim
{
class EventAction;

/**
 * @class SteppingAction
 * @brief Class for defining the stepping action in the simulation.
 *
 * This class inherits from G4UserSteppingAction and is responsible for defining the actions to be taken at each step of the simulation.
 * It is used in conjunction with the EventAction class to perform specific actions during the simulation.
 */
class SteppingAction : public G4UserSteppingAction
{
  public:
    SteppingAction(EventAction* eventAction);
    ~SteppingAction();// override = default;

    // method from the base class
    void UserSteppingAction(const G4Step*) override;
    void SetVerbosity(G4int level) { verbosityLevel = level; }  
    void Print(const G4Step* step);

  private:
    EventAction* fEventAction;
    G4int verbosityLevel=0;
};

} // namespace G4Sim

#endif
