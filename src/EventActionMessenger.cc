#include "EventAction.hh"
#include "EventActionMessenger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UImanager.hh"
#include "G4UnitsTable.hh"

/**
 * @class EventActionMessenger
 * 
 * @brief Messenger class for EventAction.
 * 
 * This class is responsible for handling user commands related to EventAction.
 * It provides commands to set the spatial and time thresholds for clustering.
 */
namespace G4Sim {

/**
 * @brief Constructs an EventActionMessenger object.
 * 
 * @param eventAction Pointer to the EventAction object.
 */
EventActionMessenger::EventActionMessenger(EventAction* eventAction)
    : G4UImessenger(), fEventAction(eventAction) {
    
    // Command to set the spatial threshold
    fSpatialThresholdCmd = new G4UIcmdWithADoubleAndUnit("/event/spatialThreshold", this);
    fSpatialThresholdCmd->SetGuidance("Set the spatial threshold for clustering.");
    fSpatialThresholdCmd->SetParameterName("SpatialThreshold", false);
    fSpatialThresholdCmd->SetRange("SpatialThreshold>=0.");
    fSpatialThresholdCmd->SetUnitCategory("Length");
    fSpatialThresholdCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
    
    // Command to set the time threshold
    fTimeThresholdCmd = new G4UIcmdWithADoubleAndUnit("/event/timeThreshold", this);
    fTimeThresholdCmd->SetGuidance("Set the time threshold for clustering.");
    fTimeThresholdCmd->SetParameterName("TimeThreshold", false);
    fTimeThresholdCmd->SetRange("TimeThreshold>=0.");
    fTimeThresholdCmd->SetUnitCategory("Time");
    fTimeThresholdCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
}

EventActionMessenger::~EventActionMessenger() {
    delete fSpatialThresholdCmd;
    delete fTimeThresholdCmd;
}

/**
 * @brief Sets the new value for a given command.
 * 
 * This method is called when a new value is set for a command in the EventActionMessenger class.
 * It checks the command and updates the corresponding value in the EventAction class.
 * 
 * @param command The G4UIcommand object representing the command.
 * @param newValue The new value to be set.
 */
void EventActionMessenger::SetNewValue(G4UIcommand* command, G4String newValue) {
    if (command == fSpatialThresholdCmd) {
        fEventAction->SetSpatialThreshold(fSpatialThresholdCmd->GetNewDoubleValue(newValue));
    } else if (command == fTimeThresholdCmd) {
        fEventAction->SetTimeThreshold(fTimeThresholdCmd->GetNewDoubleValue(newValue));
    }
}

} // namespace G4XamsSim
