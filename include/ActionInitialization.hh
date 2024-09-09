#ifndef B1ActionInitialization_h
#define B1ActionInitialization_h 1

#include "G4VUserActionInitialization.hh"
#include "EventAction.hh"
#include "RunAction.hh"
#include "SteppingAction.hh"

/**
 * @namespace G4Sim
 * @brief Namespace for the G4Sim library.
/*/
namespace G4Sim
{

/**
 * @class ActionInitialization
 * @brief Initializes user actions for the Geant4 simulation.
 *
 * This class inherits from G4VUserActionInitialization and is responsible for
 * setting up the actions that will be used during the simulation. It provides
 * methods to build actions for both master and worker threads.
 *
 * @note The destructor is defined as default.
 */
class ActionInitialization : public G4VUserActionInitialization
{
  public:
    ActionInitialization();
    ~ActionInitialization() override = default;

    void BuildForMaster() const override;
    void Build() const override;
  private:
};

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
