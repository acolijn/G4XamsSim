#include "DetectorConstructionMessenger.hh"
#include "DetectorConstruction.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
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

    fFiducialRadiusCmd = new G4UIcmdWithADoubleAndUnit("/detector/setFiducialRadius", this);
    fFiducialRadiusCmd->SetGuidance("Set the radius of the fiducial volume");
    fFiducialRadiusCmd->SetParameterName("Radius", false);
    fFiducialRadiusCmd->SetUnitCategory("Length");
    fFiducialRadiusCmd->SetDefaultValue(0.8 * m);

    fFiducialHeightCmd = new G4UIcmdWithADoubleAndUnit("/detector/setFiducialHeight", this);
    fFiducialHeightCmd->SetGuidance("Set the height of the fiducial volume");
    fFiducialHeightCmd->SetParameterName("Height", false);
    fFiducialHeightCmd->SetUnitCategory("Length");
    fFiducialHeightCmd->SetDefaultValue(1.0 * m);
}

DetectorConstructionMessenger::~DetectorConstructionMessenger() {
    delete fOuterCryostatRadiusCmd;
    delete fOuterCryostatHeightCmd;
    delete fOuterCryostatWallThicknessCmd;
    delete fInnerCryostatRadiusCmd;
    delete fInnerCryostatHeightCmd;
    delete fInnerCryostatWallThicknessCmd;
    delete fFiducialRadiusCmd;
    delete fFiducialHeightCmd;
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

    if (command == fOuterCryostatRadiusCmd) {
        fDetectorConstruction->SetOuterCryostatRadius(fOuterCryostatRadiusCmd->GetNewDoubleValue(newValue));
    } else if (command == fOuterCryostatHeightCmd) {
        fDetectorConstruction->SetOuterCryostatHeight(fOuterCryostatHeightCmd->GetNewDoubleValue(newValue));
    } else if (command == fOuterCryostatWallThicknessCmd) {
        fDetectorConstruction->SetOuterCryostatWallThickness(fOuterCryostatWallThicknessCmd->GetNewDoubleValue(newValue));
    } else if (command == fInnerCryostatRadiusCmd) {
        fDetectorConstruction->SetInnerCryostatRadius(fInnerCryostatRadiusCmd->GetNewDoubleValue(newValue));
    } else if (command == fInnerCryostatHeightCmd) {
        fDetectorConstruction->SetInnerCryostatHeight(fInnerCryostatHeightCmd->GetNewDoubleValue(newValue));
    } else if (command == fInnerCryostatWallThicknessCmd) {
        fDetectorConstruction->SetInnerCryostatWallThickness(fInnerCryostatWallThicknessCmd->GetNewDoubleValue(newValue));
    } else if (command == fFiducialRadiusCmd) {
        fDetectorConstruction->SetFiducialRadius(fFiducialRadiusCmd->GetNewDoubleValue(newValue));
    } else if (command == fFiducialHeightCmd) {
        fDetectorConstruction->SetFiducialHeight(fFiducialHeightCmd->GetNewDoubleValue(newValue));
    }
}

}