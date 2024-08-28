#include "RunActionMessenger.hh"
#include "RunAction.hh"

///using namespace G4FastSim;
namespace G4XamsSim {

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

void RunActionMessenger::SetNewValue(G4UIcommand* command, G4String newValue) {
    if (command == fOutputFileNameCmd) {
        fRunAction->SetOutputFileName(newValue);
    }
}

} // namespace G4FastSim