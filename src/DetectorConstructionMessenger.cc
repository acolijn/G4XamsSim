#include "DetectorConstructionMessenger.hh"
#include "DetectorConstruction.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithAString.hh"
#include "G4SystemOfUnits.hh"

namespace G4XamsSim{

/**
 * @brief Constructor for the DetectorConstructionMessenger class.
 * 
 * @param detector A pointer to the DetectorConstruction object.
 */
DetectorConstructionMessenger::DetectorConstructionMessenger(DetectorConstruction* detector)
    : fDetectorConstruction(detector) {

    G4UIdirectory* dir = new G4UIdirectory("/detector/");
    dir->SetGuidance("UI commands for the detector setup");

    fGeometryFileNameCmd = new G4UIcmdWithAString("/detector/setGeometryFileName", this);
    fGeometryFileNameCmd->SetGuidance("Set the geometry JSON file name.");
    fGeometryFileNameCmd->SetParameterName("fileName", false);
    fGeometryFileNameCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
}


DetectorConstructionMessenger::~DetectorConstructionMessenger() {
    delete fGeometryFileNameCmd;
}

/**
 * @brief Sets the new value for a given command.
 *
 * This function is called when a new value is assigned to a command in the user interface.
 * It checks which command is being modified and calls the corresponding setter function in the `DetectorConstruction` class.
 *
 * @param command The command being modified.
 * @param newValue The new value assigned to the command.
 */
void DetectorConstructionMessenger::SetNewValue(G4UIcommand* command, G4String newValue) {

    if (command == fGeometryFileNameCmd) {
        fDetectorConstruction->SetGeometryFileName(newValue);
    }
}

} // namespace G4XamsSim