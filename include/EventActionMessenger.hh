#ifndef EVENTACTIONMESSENGER_H
#define EVENTACTIONMESSENGER_H

#include "G4UImessenger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithABool.hh"
#include "EventAction.hh"

namespace G4XamsSim {

class EventAction;

class EventActionMessenger : public G4UImessenger {
public:
    EventActionMessenger(EventAction* eventAction);
    ~EventActionMessenger() override;

    void SetNewValue(G4UIcommand* command, G4String newValue) override;

private:
    EventAction* fEventAction;

    G4UIcmdWithADoubleAndUnit* fSpatialThresholdCmd;
    G4UIcmdWithADoubleAndUnit* fTimeThresholdCmd;

};

} // namespace G4XamsSim

#endif
