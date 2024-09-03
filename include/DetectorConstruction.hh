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
//
/// \file B1/include/DetectorConstruction.hh
/// \brief Definition of the B1::DetectorConstruction class

#ifndef __DetectorConstruction__
#define __DetectorConstruction__ 1

#include "G4VUserDetectorConstruction.hh"
#include "DetectorConstructionMessenger.hh"
#include "G4UnionSolid.hh"
#include "Materials.hh"
#include "globals.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;
class DetectorConstructionMessenger;

/// Detector construction class to define materials and geometry.

namespace G4XamsSim
{

class DetectorConstruction : public G4VUserDetectorConstruction
{
  public:
    DetectorConstruction();
    ~DetectorConstruction();//override = default;

    G4VPhysicalVolume* Construct() override;
    void ConstructWorld();
    void ConstructOuterCryostat();
    void ConstructInnerCryostat();    
    void ConstructGXe();
    void ConstructLXe();
    void ConstructTPC();

    void ConstructFiducialVolume();
    void DefineSensitiveDetector();

    G4LogicalVolume* GetScoringVolume() const { return fScoringVolume; }

    void SetOuterCryostatRadius(G4double radius) { outer_cryostat_radius = radius; }
    void SetOuterCryostatHeight(G4double height) { outer_cryostat_height = height; }
    void SetOuterCryostatWallThickness(G4double thickness) { outer_cryostat_wall_thickness = thickness; }
    void SetInnerCryostatRadius(G4double radius) { inner_cryostat_radius = radius; }
    void SetInnerCryostatHeight(G4double height) { inner_cryostat_height = height; }
    void SetInnerCryostatWallThickness(G4double thickness) { inner_cryostat_wall_thickness = thickness; }
    void SetFiducialRadius(G4double radius) { fiducial_radius = radius; }
    void SetFiducialHeight(G4double height) { fiducial_height = height; }

    G4UnionSolid* InnerVessel();
    G4UnionSolid* OuterVessel();

  private:
    Materials *fMaterials = nullptr;

    // check for overlaps
    G4bool fCheckOverlaps = true;

    //dimensions
    G4double outer_cryostat_radius;
    G4double outer_cryostat_height;
    G4double outer_cryostat_wall_thickness;
    G4double outer_cryostat_flange_radius;
    G4double outer_cryostat_flange_thickness;

    G4double inner_cryostat_radius;
    G4double inner_cryostat_height;
    G4double inner_cryostat_wall_thickness;
    G4double inner_cryostat_flange_radius;
    G4double inner_cryostat_flange_thickness;

    G4double cryostat_z_offset;

    G4double fiducial_radius;
    G4double fiducial_height;

    //logical volumes
    G4LogicalVolume* fWorldLogical = nullptr;
    G4LogicalVolume* fOuterCryostatLogical = nullptr;
    G4LogicalVolume* fVacuumLogical = nullptr;
    G4LogicalVolume* fInnerCryostatLogical = nullptr;
    G4LogicalVolume* fGXeLogical = nullptr;
    G4LogicalVolume* fPTFECupLogical = nullptr;
    G4LogicalVolume* fPTFECylinderLogical = nullptr;

    G4LogicalVolume* fLXeLogical = nullptr;
    G4LogicalVolume* fPTFELogical = nullptr;
    G4LogicalVolume* fLXeFiducialLogical = nullptr;

    //physical volumes
    G4VPhysicalVolume* fWorldPhysical = nullptr;
    G4VPhysicalVolume* fOuterCryostatPhysical = nullptr;
    G4VPhysicalVolume* fVacuumPhysical = nullptr;
    G4VPhysicalVolume* fInnerCryostatPhysical = nullptr;
    G4VPhysicalVolume* fLXePhysical = nullptr;
    G4VPhysicalVolume* fGXePhysical = nullptr;
    G4VPhysicalVolume* fPTFECupPhysical = nullptr;
    G4VPhysicalVolume* fPTFECylinderPhysical = nullptr;f

    G4VPhysicalVolume* fPTFEPhysical = nullptr;
    G4VPhysicalVolume* fLXeFiducialPhysical = nullptr;

  protected:
    G4LogicalVolume* fScoringVolume = nullptr;

    DetectorConstructionMessenger* fMessenger;

};

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
