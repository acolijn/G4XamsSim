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
/// \file B1/src/DetectorConstruction.cc
/// \brief Implementation of the B1::DetectorConstruction class

#include "DetectorConstruction.hh"
#include "DetectorConstructionMessenger.hh"
#include "SensitiveDetector.hh"
#include "Materials.hh"

#include "G4RunManager.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"	
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4Material.hh"
#include "G4AnalysisManager.hh"
#include "G4SDManager.hh"

namespace G4XamsSim{
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::DetectorConstruction()
  : G4VUserDetectorConstruction(){

  // Set XAMS values
  outer_cryostat_radius = 254.0 / 2. * mm;
  outer_cryostat_height = 470.0 * mm;
  outer_cryostat_wall_thickness = 2.77 * mm;
  outer_cryostat_flange_thickness = 12.0 * mm;
  outer_cryostat_flange_radius = 290.0 / 2. * mm;
  outer_cryostat_flange_thickness = 12.0 * mm;

  inner_cryostat_radius = 152.4 / 2. * mm;
  inner_cryostat_height = 331.0 * mm;
  inner_cryostat_wall_thickness = 2.77 * mm;
  inner_cryostat_flange_radius =  202.5/ 2. * mm;
  inner_cryostat_flange_thickness = 22.0 * mm;

  // z offset for outer cryostat with respect to inner cryostat
  // calculated from the drawings of our setup
  // the inner cryostat in this way is centered at z=0
  cryostat_z_offset = 26*mm;

  fiducial_radius = 10 * mm;
  fiducial_height = 10 * mm;

  // Create the messenger class
  // This class is responsible for handling user input commands
  // and updating the detector geometry accordingly
  fMessenger = new DetectorConstructionMessenger(this);

}

DetectorConstruction::~DetectorConstruction()
{
  delete fMessenger;
}

/**
 * @brief Constructs the detector geometry.
 * 
 * This function is responsible for constructing the detector geometry by defining materials,
 * constructing various volumes such as the world volume, cryostats, liquid xenon,
 * and fiducial volume. It also initializes the gamma ray helper class with the defined materials.
 * 
 * @return The physical volume of the world.
 */
G4VPhysicalVolume* DetectorConstruction::Construct()
{
  //fGammaRayHelper = &GammaRayHelper::Instance();
  //
  // Verification of geometry
  //
  fCheckOverlaps = true;
  //
  // Define materials
  //
  fMaterials = new Materials();
  fMaterials->DefineMaterials();
  // 
  // Construct world volume
  //
  ConstructWorld();
  //
  // Construct outer cryostat and fill with vacuum
  //
  ConstructOuterCryostat();
  //
  // Construct inner cryostat 
  //
  ConstructInnerCryostat();
  //
  //
  //
  ConstructTPC();
  //
  // Define the inner fiducial volume
  //
  ConstructFiducialVolume();
  // 
  // Define the sensitive detector
  //
  DefineSensitiveDetector();
  //
  //always return the physical World
  //
  return fWorldPhysical;
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void DetectorConstruction::DefineSensitiveDetector(){
  //
  // Define the sensitive detector
  //

  G4SDManager* sdManager = G4SDManager::GetSDMpointer();
  // // auto* lXeSD = new G4FastSim::SensitiveDetector("LXeSD","LXeCollection");
  auto* lXeFiducialSD = new G4XamsSim::SensitiveDetector("LXeFiducialSD","LXeFiducialCollection");

  // make both the liquid xenon and the fiducial volume sensitive
  // this is only relevant for the standard Monte Carlo. The fast simulation will 
  // use the energy deposition directly.
  // // sdManager->AddNewDetector(lXeSD);
  sdManager->AddNewDetector(lXeFiducialSD);

  // // fLXeLogical->SetSensitiveDetector(lXeSD);
  fLXeFiducialLogical->SetSensitiveDetector(lXeFiducialSD);
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void DetectorConstruction::ConstructWorld(){
  //
  // Construct the world volume
  //
  G4double world_sizeXY = 1*m, world_sizeZ = 1*m;
  
  G4Material* world_mat = G4Material::GetMaterial("G4_AIR");
  auto solidWorld = new G4Box("World", 0.5 * world_sizeXY, 0.5 * world_sizeXY, 0.5 * world_sizeZ);
  fWorldLogical = new G4LogicalVolume(solidWorld, world_mat, "World");

  fWorldPhysical = new G4PVPlacement(nullptr,  // no rotation
    G4ThreeVector(),                           // at (0,0,0)
    fWorldLogical,                                // its logical volume
    "World",                                   // its name
    nullptr,                                   // its mother  volume
    false,                                     // no boolean operation
    0,                                         // copy number
    fCheckOverlaps);                            // overlaps checking

}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4UnionSolid* DetectorConstruction::OuterVessel(){
  // Make a solid stainless steel outer vessel
  // The vessel will be extended to the bottom and top with a flange....
  G4UnionSolid* outerVessel;

  auto solidCylinder = new G4Tubs("cylinder", 0., outer_cryostat_radius+outer_cryostat_wall_thickness, outer_cryostat_height / 2., 0. * deg, 360. * deg);
  // the thickness of the bottom/top flange does not need to be divided by two, since there is a ring on the cylinder part
  // and a flange on the bottom part
  auto solidBottomFlange = new G4Tubs("bottomFlange", 0., outer_cryostat_flange_radius, outer_cryostat_flange_thickness , 0. * deg, 360. * deg);
  auto solidTopFlange = new G4Tubs("topFlange", 0., outer_cryostat_flange_radius, outer_cryostat_flange_thickness, 0. * deg, 360. * deg);

  outerVessel = new G4UnionSolid("outerVessel", solidCylinder, solidBottomFlange, 0, G4ThreeVector(0., 0., -outer_cryostat_height / 2.));
  outerVessel = new G4UnionSolid("outerVessel", outerVessel, solidTopFlange, 0, G4ThreeVector(0., 0., outer_cryostat_height / 2.));

  return outerVessel;
}

void DetectorConstruction::ConstructOuterCryostat(){
  //
  // Outer cryostat filled with vacuum.....
  //

  G4cout << "DetectorConstruction::ConstructOuterCryostat Constructing outer cryostat" << G4endl;	
  G4cout << "DetectorConstruction::ConstructOuterCryostat outer_cryostat_radius = " << outer_cryostat_radius/cm << " cm"<<G4endl;
  G4cout << "DetectorConstruction::ConstructOuterCryostat outer_cryostat_height = " << outer_cryostat_height/cm << " cm"<<G4endl;
  G4cout << "DetectorConstruction::ConstructOuterCryostat outer_cryostat_wall_thickness = " << outer_cryostat_wall_thickness/cm << " cm"<<G4endl;
  G4cout << "DetectorConstruction::ConstructOuterCryostat outer_cryostat_flange_radius = " << outer_cryostat_flange_radius/cm << " cm"<<G4endl;
  G4cout << "DetectorConstruction::ConstructOuterCryostat outer_cryostat_flange_thickness = " << outer_cryostat_flange_thickness/cm << " cm"<<G4endl;

  G4Material* stainless_steel = G4Material::GetMaterial("StainlessSteel");


  //auto solidOuterCryostat = new G4Tubs("OuterCryostat",           // its name
  //  0, outer_cryostat_radius, outer_cryostat_height / 2., 0. * deg, 360. * deg);  // its size
  auto solidOuterCryostat = OuterVessel();

  fOuterCryostatLogical = new G4LogicalVolume(solidOuterCryostat,  // its solid
    stainless_steel,                                         // its material
    "OuterCryostat");                                    // its name

  fOuterCryostatPhysical = new G4PVPlacement(nullptr,  // no rotation
    G4ThreeVector(0,0,0),          // at (0,0,0)
    fOuterCryostatLogical,     // its logical volume
    "OuterCryostat",          // its name
    fWorldLogical,        // its mother  volume
    false,                    // no boolean operation
    0,                        // copy number
    fCheckOverlaps);           // overlaps checking

  // Fill the outer cryostat with vacuum
  G4Material* vacuum = G4Material::GetMaterial("Vacuum");
  auto solidVacuum = new G4Tubs("Vacuum",           // its name
    0., outer_cryostat_radius, outer_cryostat_height / 2., 0. * deg, 360. * deg);  // its size  

  fVacuumLogical = new G4LogicalVolume(solidVacuum,  // its solid
    vacuum,                                         // its material
    "Vacuum");                                    // its name

  fVacuumPhysical = new G4PVPlacement(nullptr,  // no rotation
    G4ThreeVector(0,0,0),          // at (0,0,0) wrt to the outer cryostat
    fVacuumLogical,             // its logical volume
    "Vacuum",                 // its name
    fOuterCryostatLogical,     // its mother  volume
    false,                    // no boolean operation
    0,                        // copy number
    fCheckOverlaps);           // overlaps checking

}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
G4UnionSolid* DetectorConstruction::InnerVessel(){
  // Make a solid stainless steel inner vessel
  // The vessel will be extended to the bottom and top with a flange....
  G4UnionSolid* innerVessel;

  auto solidCylinder = new G4Tubs("cylinder", 0., inner_cryostat_radius+inner_cryostat_wall_thickness, inner_cryostat_height / 2., 0. * deg, 360. * deg);
  // the thickness of the bottom/top flange does not need to be divided by two, since there is a ring on the cylinder part
  // and a flange on the bottom part
  auto solidBottomFlange = new G4Tubs("bottomFlange", 0., inner_cryostat_flange_radius, inner_cryostat_flange_thickness , 0. * deg, 360. * deg);
  auto solidTopFlange = new G4Tubs("topFlange", 0., inner_cryostat_flange_radius, inner_cryostat_flange_thickness, 0. * deg, 360. * deg);

  innerVessel = new G4UnionSolid("innerVessel", solidCylinder, solidBottomFlange, 0, G4ThreeVector(0., 0., -inner_cryostat_height / 2.));
  innerVessel = new G4UnionSolid("innerVessel", innerVessel, solidTopFlange, 0, G4ThreeVector(0., 0., inner_cryostat_height / 2.));

  return innerVessel;
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

/**
 * Constructs the inner cryostat and fills it with LXe.
 */
void DetectorConstruction::ConstructInnerCryostat(){
  //
  // Construct inner cryostat and fill with GXe
  //

  // the vessel

  G4cout << "DetectorConstruction::ConstructInnerCryostat Constructing inner cryostat" << G4endl;
  G4cout << "DetectorConstruction::ConstructInnerCryostat inner_cryostat_radius = " << inner_cryostat_radius/cm << " cm"<<G4endl;
  G4cout << "DetectorConstruction::ConstructInnerCryostat inner_cryostat_height = " << inner_cryostat_height/cm << " cm"<<G4endl;
  G4cout << "DetectorConstruction::ConstructInnerCryostat inner_cryostat_wall_thickness = " << inner_cryostat_wall_thickness/cm << " cm"<<G4endl;
  G4cout << "DetectorConstruction::ConstructInnerCryostat inner_cryostat_flange_radius = " << inner_cryostat_flange_radius/cm << " cm"<<G4endl;
  G4cout << "DetectorConstruction::ConstructInnerCryostat inner_cryostat_flange_thickness = " << inner_cryostat_flange_thickness/cm << " cm"<<G4endl;

  G4Material* stainless_steel = G4Material::GetMaterial("StainlessSteel");
  //auto solidInnerCryostat = new G4Tubs("InnerCryostat",           // its name
  //  0., inner_cryostat_radius, inner_cryostat_height / 2., 0. * deg, 360. * deg);  // its size
  auto solidInnerCryostat = InnerVessel();

  fInnerCryostatLogical = new G4LogicalVolume(solidInnerCryostat,  // its solid
    stainless_steel,                                         // its material
    "InnerCryostat");                                    // its name

  fInnerCryostatPhysical = new G4PVPlacement(nullptr,  // no rotation
    G4ThreeVector(0,0,-cryostat_z_offset),          // at (0,0,0)
    fInnerCryostatLogical,     // its logical volume
    "InnerCryostat",          // its name
    fVacuumLogical,            // its mother  volume
    false,                    // no boolean operation
    0,                        // copy number
    fCheckOverlaps);           // overlaps checking

  // Fill the inner cryostat with GXe
  ConstructGXe();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void DetectorConstruction::ConstructGXe(){
  G4cout << "DetectorConstruction::ConstructLXe Constructing GXe" << G4endl;

  G4Material* gXe = G4Material::GetMaterial("GXe");
  auto solidGXe = new G4Tubs("GXe",           // its name
    0., inner_cryostat_radius, inner_cryostat_height / 2., 0. * deg, 360. * deg);  // its size

  fGXeLogical = new G4LogicalVolume(solidGXe,  // its solid
    gXe,                                         // its material
    "GXe");                                    // its name

  fGXePhysical = new G4PVPlacement(nullptr,  // no rotation
    G4ThreeVector(),          // at (0,0,0)
    fGXeLogical,               // its logical volume
    "GXe",                    // its name
    fInnerCryostatLogical,     // its mother  volume
    false,                    // no boolean operation
    0,                        // copy number
    fCheckOverlaps);           // overlaps checking
}

void DetectorConstruction::ConstructTPC(){
  G4cout << "DetectorConstruction::ConstructTPC Constructing TPC" << G4endl;

  // 1. PTFE cup that holds the LXe
  G4double ptfe_cup_radius = 55.0 * mm;
  G4double ptfe_cup_thickness = 10.0 * mm;
  G4double ptfe_cup_height = 110.0 * mm;

  G4Material* PTFE = G4Material::GetMaterial("PTFE"); 
  auto solidPTFE1 = new G4Tubs("PTFE1",           // its name
    ptfe_cup_radius, ptfe_cup_radius + ptfe_cup_thickness, ptfe_cup_height / 2., 0. * deg, 360. * deg);  // its size  
  auto solidPTFE2 = new G4Tubs("PTFE2",           // its name
    0., ptfe_cup_radius+ptfe_cup_thickness, ptfe_cup_thickness / 2., 0. * deg, 360. * deg);  // its size
  auto solidPTFE = new G4UnionSolid("PTFE", solidPTFE1, solidPTFE2, 0, 
    G4ThreeVector(0., 0., -ptfe_cup_height / 2. - ptfe_cup_thickness / 2.));
  //auto solidPTFE = new G4Tubs("PTFE
  fPTFECupLogical = new G4LogicalVolume(solidPTFE,  // its solid
    PTFE,                                         // its material
    "PTFECup");                                    // its name
  
  G4double z_PTFECup = -10. * mm;
  fPTFECupPhysical = new G4PVPlacement(nullptr,  // no rotation
    G4ThreeVector(0,0,z_PTFECup),          // at (0,0,0)
    fPTFECupLogical,     // its logical volume
    "PTFECup",          // its name
    fGXeLogical,            // its mother  volume
    false,                    // no boolean operation
    0,                        // copy number
    fCheckOverlaps);           // overlaps checking

  // 2. fill PTFE cup with LXe.... note that the cup is an empty shell
  // so this volume is added to the GXe Logical volume
  G4double LXe_height = 100.0 * mm;
  G4Material* lXe = G4Material::GetMaterial("LXe");
  auto solidLXe = new G4Tubs("LXe",           // its name
    0., ptfe_cup_radius, LXe_height / 2., 0. * deg, 360. * deg);  // its size 

  fLXeLogical = new G4LogicalVolume(solidLXe,  // its solid
    lXe,                                         // its material
    "LXe");                                    // its name

  G4double z_offset = z_PTFECup - ptfe_cup_height / 2. + LXe_height / 2.;
  fLXePhysical = new G4PVPlacement(nullptr,  // no rotation
    G4ThreeVector(0,0,z_offset),          // at (0,0,0)
    fLXeLogical,               // its logical volume
    "LXe",                    // its name
    fGXeLogical,     // its mother  volume
    false,                    // no boolean operation
    0,                        // copy number
    fCheckOverlaps);           // overlaps checking
  
  // 3. place the PTFE reflection cylinder inside the LXe 
  G4double reflection_cylinder_radius = 62.0/2 * mm;
  G4double reflection_cylinder_thickness = 5.0 * mm;
  G4double reflection_cylinder_height = 72.0 * mm;
  auto solidReflectionCylinder = new G4Tubs("ReflectionCylinder",           // its name
    reflection_cylinder_radius, reflection_cylinder_radius + reflection_cylinder_thickness, reflection_cylinder_height / 2., 0. * deg, 360. * deg);  // its size  

  fPTFECylinderLogical = new G4LogicalVolume(solidReflectionCylinder,  // its solid
    PTFE,                                         // its material
    "PTFECylinder");                                    // its name

  // I want to place the cylinder inside the liquid xenon, such tht the top is 3 mm blow the surface
  G4double z_offset_cylinder =  LXe_height / 2. - reflection_cylinder_height / 2. - 3.0 * mm;
  
  fPTFECylinderPhysical = new G4PVPlacement(nullptr,  // no rotation
    G4ThreeVector(0,0,z_offset_cylinder),          // at (0,0,0)
    fPTFECylinderLogical,     // its logical volume
    "PTFECylinder",          // its name
    fLXeLogical,            // its mother  volume
    false,                    // no boolean operation
    0,                        // copy number
    fCheckOverlaps);           // overlaps checking

  // 4. place the otehr stuff inside also....... if you like

}
/**
 * Constructs the liquid xenon volume in the detector.
 */
void DetectorConstruction::ConstructLXe(){
  // the liquid xenon volume

  G4cout << "DetectorConstruction::ConstructLXe Constructing LXe" << G4endl;

  G4Material* lXe = G4Material::GetMaterial("LXe");
  auto solidLXe = new G4Tubs("LXe",           // its name
    0., inner_cryostat_radius, inner_cryostat_height / 2., 0. * deg, 360. * deg);  // its size

  fLXeLogical = new G4LogicalVolume(solidLXe,  // its solid
    lXe,                                         // its material
    "LXe");                                    // its name

  fLXePhysical = new G4PVPlacement(nullptr,  // no rotation
    G4ThreeVector(),          // at (0,0,0)
    fLXeLogical,               // its logical volume
    "LXe",                    // its name
    fInnerCryostatLogical,     // its mother  volume
    false,                    // no boolean operation
    0,                        // copy number
    fCheckOverlaps);           // overlaps checking
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
/**
 * Constructs the liquid xenon fiducial volume.
 */
void DetectorConstruction::ConstructFiducialVolume(){

  // the liquid xenon fiducial volume

  G4cout << "DetectorConstruction::ConstructFiducialVolume Constructing LXe fiducial volume" << G4endl;
  G4cout << "DetectorConstruction::ConstructFiducialVolume fiducial_radius = " << fiducial_radius/cm << " cm"<<G4endl;
  G4cout << "DetectorConstruction::ConstructFiducialVolume fiducial_height = " << fiducial_height/cm << " cm"<<G4endl;

  G4Material* lXe = G4Material::GetMaterial("LXe");

  auto solidLXeFiducial = new G4Tubs("LXeFiducial",           // its name
    0., fiducial_radius, fiducial_height / 2., 0. * deg, 360. * deg);  // its size

  fLXeFiducialLogical = new G4LogicalVolume(solidLXeFiducial,  // its solid
    lXe,                                         // its material
    "LXeFiducial");                                    // its name

  fLXeFiducialPhysical = new G4PVPlacement(nullptr,  // no rotation
    G4ThreeVector(),          // at (0,0,0)
    fLXeFiducialLogical,       // its logical volume
    "LXeFiducial",            // its name
    fLXeLogical,               // its mother  volume
    false,                    // no boolean operation
    0,                        // copy number
    fCheckOverlaps);           // overlaps checking 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}

}