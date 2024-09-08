//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
/// \file B1/include/DetectorConstruction.hh
/// \brief Definition of the G4XamsSim::DetectorConstruction class

#ifndef DETECTOR_CONSTRUCTION_HH
#define DETECTOR_CONSTRUCTION_HH

#include "G4VUserDetectorConstruction.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4VSolid.hh"
#include "DetectorConstructionMessenger.hh"
#include "Materials.hh"

#include "nlohmann/json.hpp"

#include <map>
#include <string>

namespace G4XamsSim {

class DetectorConstruction : public G4VUserDetectorConstruction {
public:
    DetectorConstruction();
    ~DetectorConstruction() override;

    G4VPhysicalVolume* Construct() override;
        
    // set the JSON geometry file name
    void SetGeometryFileName(const std::string& fileName);

private:
    void LoadGeometryFromJson(const std::string& jsonFileName);
    void MakeVolumeSensitive(const G4String& volumeName, const G4String& collectionName);
    G4LogicalVolume* ConstructVolume(const nlohmann::json& volumeDef);
    G4VSolid* CreateSolid(const nlohmann::json& solidDef);
    G4LogicalVolume* GetLogicalVolume(const G4String& name);

    // Maps to store logical and physical volumes for easy lookup
    std::map<G4String, G4LogicalVolume*> logicalVolumeMap;
    std::map<G4String, G4VPhysicalVolume*> physicalVolumeMap;

    // World physical and logical volumes
    G4LogicalVolume* fWorldLogical = nullptr;
    G4VPhysicalVolume* fWorldPhysical = nullptr;

    // Flag to check overlaps
    G4bool fCheckOverlaps = true;
    Materials *fMaterials = nullptr;

    std::string jsonFileName;

    DetectorConstructionMessenger* fMessenger;
};

}  // namespace G4XamsSim

#endif
