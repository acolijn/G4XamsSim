#ifndef _DETECTORCONSTRUCTIONMESSENGER_HH
#define _DETECTORCONSTRUCTIONMESSENGER_HH

#include "G4UImessenger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "globals.hh"


namespace G4XamsSim {

class DetectorConstruction;

class DetectorConstructionMessenger : public G4UImessenger {
    public:
        DetectorConstructionMessenger(DetectorConstruction* detector);
        ~DetectorConstructionMessenger();

        void SetNewValue(G4UIcommand* command, G4String newValue) override;

    private:
        DetectorConstruction* fDetectorConstruction;

        G4UIcmdWithADoubleAndUnit* fFiducialRadiusCmd;
        G4UIcmdWithADoubleAndUnit* fFiducialHeightCmd;
};

}
#endif // DETECTORCONSTRUCTIONMESSENGER_HH
