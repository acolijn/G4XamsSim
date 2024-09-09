#include "RunActionMessenger.hh"
#include "RunAction.hh"

///using namespace G4FastSim;
namespace G4Sim {

/**
 * @brief Constructs a RunActionMessenger object.
 * 
 * @param action Pointer to the RunAction object.
 */
RunActionMessenger::RunActionMessenger(RunAction* action)
    : fRunAction(action) {

    fOutputFileNameCmd = new G4UIcmdWithAString("/run/setOutputFileName", this);
    fOutputFileNameCmd->SetGuidance("Set the output file name.");
    fOutputFileNameCmd->SetParameterName("outputFileName", false);
    fOutputFileNameCmd->SetDefaultValue("G4XamsSim.root");

}

RunActionMessenger::~RunActionMessenger() {
    delete fOutputFileNameCmd;
}

/**
 * @brief Sets the new value for a given G4UIcommand.
 *
 * This method is called when a G4UIcommand is executed and the new value is passed as a G4String.
 * It checks if the given command is the fOutputFileNameCmd and if so, it sets the new value as the output file name for the associated RunAction object.
 *
 * @param command The G4UIcommand object.
 * @param newValue The new value as a G4String.
 */
void RunActionMessenger::SetNewValue(G4UIcommand* command, G4String newValue) {
    if (command == fOutputFileNameCmd) {
        fRunAction->SetOutputFileName(newValue);
    }
}

} // namespace G4Sim