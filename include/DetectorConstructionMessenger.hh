#ifndef _DETECTORCONSTRUCTIONMESSENGER_HH
#define _DETECTORCONSTRUCTIONMESSENGER_HH

#include "G4UImessenger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithAString.hh"
#include "globals.hh"

/**
 * @namespace G4Sim
 * @brief Namespace for the G4Sim library.
/*/
namespace G4Sim {

class DetectorConstruction;

class DetectorConstructionMessenger : public G4UImessenger {
    public:
        DetectorConstructionMessenger(DetectorConstruction* detector);
        ~DetectorConstructionMessenger();

        void SetNewValue(G4UIcommand* command, G4String newValue) override;

    private:
        DetectorConstruction* fDetectorConstruction;
        G4UIcmdWithAString* fGeometryFileNameCmd;  // New command to set geometry file name
};

}
#endif // DETECTORCONSTRUCTIONMESSENGER_HH
