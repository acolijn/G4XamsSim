#ifndef SteppingAction_h
#define SteppingAction_h 1

#include "G4UserSteppingAction.hh"
#include "G4ParticleTable.hh"
#include "globals.hh"

class G4LogicalVolume;

/// Stepping action class
///

namespace G4XamsSim
{
class EventAction;

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

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
