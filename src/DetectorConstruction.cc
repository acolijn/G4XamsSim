#include "DetectorConstruction.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4UnionSolid.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4LogicalVolume.hh"
#include "G4NistManager.hh"
#include "G4Material.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4PVPlacement.hh"
#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"
#include "G4VisAttributes.hh"
#include "G4SystemOfUnits.hh"

#include "DetectorConstructionMessenger.hh"
#include "Materials.hh"
#include "SensitiveDetector.hh"
#include "EventAction.hh"
#include "G4Material.hh"
#include "G4SDManager.hh"
#include "G4RunManager.hh"


#include "nlohmann/json.hpp"
#include <fstream>
#include <iostream>

using namespace G4XamsSim;
using json = nlohmann::json;

/**
 * @namespace G4XamsSim
 * @brief Namespace for the G4XamsSim library.
/*/
namespace G4XamsSim {

/**
 * @brief Constructor for the DetectorConstruction class.
 */
DetectorConstruction::DetectorConstruction() 
  : G4VUserDetectorConstruction() {
    // Load geometry from JSON
    fMessenger = new DetectorConstructionMessenger(this);
}

/**
 * @brief Destructor for the DetectorConstruction class.
 */
DetectorConstruction::~DetectorConstruction() {}

/**
 * @brief Constructs the physical volume of the detector.
 * 
 * This function loads the geometry from a JSON file and constructs the physical volume of the detector.
 * 
 * @return The constructed physical volume of the detector.
 */
G4VPhysicalVolume* DetectorConstruction::Construct()
{
  //const std::string& jsonFileName = "/user/z37/g4/G4XamsSim/geometry.json";
  G4cout << "Loading geometry from JSON file: " << jsonFileName << G4endl;
  LoadGeometryFromJson(jsonFileName);
  G4cout << "Geometry loaded successfully!" << G4endl;

  return fWorldPhysical;
}

/**
 * @brief Sets the geometry file name.
 * 
 * This function sets the name of the JSON file that contains the geometry information.
 * 
 * @param fileName The name of the JSON file.
 */
void DetectorConstruction::SetGeometryFileName(const std::string& fileName) {
    jsonFileName = fileName;
    G4cout << "JSON file name set to: " << jsonFileName << G4endl;
}

/**
 * Loads the geometry from a JSON file.
 * 
 * @param jsonFileName The path to the JSON file containing the geometry information.
 */
void DetectorConstruction::LoadGeometryFromJson(const std::string& jsonFileName) {

    // construct materials
    fMaterials = new Materials();
    fMaterials->DefineMaterials();

    std::ifstream inputFile(jsonFileName);
    if (!inputFile.is_open()) {
        G4cerr << "Error: Could not open geometry JSON file: " << jsonFileName << G4endl;
        exit(-1);	
    }

    json geometryJson;
    inputFile >> geometryJson;

    // First construct the world volume
    G4Material* worldMaterial = G4Material::GetMaterial("G4_AIR");
    G4double worldSize = geometryJson["world"]["size"].get<double>() * m;
    G4Box* worldBox = new G4Box("World", worldSize / 2, worldSize / 2, worldSize / 2);
    fWorldLogical = new G4LogicalVolume(worldBox, worldMaterial, "World");
    fWorldPhysical = new G4PVPlacement(nullptr, G4ThreeVector(), fWorldLogical, "World", nullptr, false, 0, fCheckOverlaps);

    // Store the world volume in the logical volume map
    logicalVolumeMap["World"] = fWorldLogical;

    // Now construct other volumes
    for (const auto& volume : geometryJson["volumes"]) {
        G4LogicalVolume* logVol = ConstructVolume(volume);
        if (logVol) {
            logicalVolumeMap[volume["name"]] = logVol;  // Store logical volume
            // If the volume is marked as active, make it a sensitive detector
            if (volume.contains("active") && volume["active"].get<bool>()) {
              G4cout << "Making volume sensitive: " << volume["name"] << G4endl;
              MakeVolumeSensitive(volume["name"].get<std::string>(), volume["name"].get<std::string>() + "Collection");
            }
        }
    }
}

/**
 * Makes a volume sensitive by assigning a sensitive detector to it.
 * 
 * @param volumeName The name of the volume to make sensitive.
 * @param collectionName The name of the collection associated with the sensitive detector.
 */
void DetectorConstruction::MakeVolumeSensitive(const G4String& volumeName, const G4String& collectionName) {
    G4SDManager* sdManager = G4SDManager::GetSDMpointer();

    // Create a new sensitive detector
    auto* sensitiveDetector = new G4XamsSim::SensitiveDetector(volumeName, collectionName);
    sdManager->AddNewDetector(sensitiveDetector);

    // Assign the sensitive detector to the corresponding logical volume
    G4LogicalVolume* logicalVolume = GetLogicalVolume(volumeName);
    if (logicalVolume) {
        logicalVolume->SetSensitiveDetector(sensitiveDetector);
        G4cout << "Assigned sensitive detector to volume: " << volumeName << G4endl;

        // Register the hits collection name with EventAction
        auto* eventAction = const_cast<EventAction*>(dynamic_cast<const EventAction*>(G4RunManager::GetRunManager()->GetUserEventAction()));

        if (eventAction) {
            eventAction->AddHitsCollectionName(collectionName);
        }
    } else {
        G4cerr << "Error: Logical volume " << volumeName << " not found!" << G4endl;
    }
}


/**
 * Constructs a G4LogicalVolume based on the provided volume definition.
 *
 * @param volumeDef The JSON object containing the volume definition.
 * @return The constructed G4LogicalVolume, or nullptr if an error occurred.
 */
G4LogicalVolume* DetectorConstruction::ConstructVolume(const json& volumeDef) {
    G4String name = volumeDef["name"].get<std::string>();
    G4Material* material = G4Material::GetMaterial(volumeDef["material"].get<std::string>());
    G4LogicalVolume* logicalVolume = nullptr;

    G4cout << "Constructing volume: " << name << G4endl;
    G4cout << "Material: " << material->GetName() << G4endl;  

    // Handle compound shapes (e.g., G4UnionSolid)
    if (volumeDef.contains("components")) {
        G4VSolid* unionSolid = nullptr;
        for (const auto& component : volumeDef["components"]) {
            G4VSolid* solid = CreateSolid(component);
            G4ThreeVector position(component["placement"]["x"].get<double>() * mm,
                                   component["placement"]["y"].get<double>() * mm,
                                   component["placement"]["z"].get<double>() * mm);
            if (!unionSolid) {
                unionSolid = solid;  // First component
            } else {
                unionSolid = new G4UnionSolid(name, unionSolid, solid, nullptr, position);
            }
        }
        logicalVolume = new G4LogicalVolume(unionSolid, material, name);
    } else {
        G4VSolid* solid = CreateSolid(volumeDef);
        logicalVolume = new G4LogicalVolume(solid, material, name);
    }

    // Place volume inside its parent
    if (logicalVolume) {
        G4LogicalVolume* parentVolume = fWorldLogical;  // Default to world

        if (volumeDef.contains("parent")) {
            G4String parentName = volumeDef["parent"].get<std::string>();
            parentVolume = GetLogicalVolume(parentName);
            if (!parentVolume) {
                G4cerr << "Error: Parent volume " << parentName << " not found!" << G4endl;
                return nullptr;
            }
        }

        G4ThreeVector position(volumeDef["placement"]["x"].get<double>() * mm, 
                               volumeDef["placement"]["y"].get<double>() * mm, 
                               volumeDef["placement"]["z"].get<double>() * mm);

        G4VPhysicalVolume* physVol = new G4PVPlacement(nullptr, position, logicalVolume, name, parentVolume, false, 0, fCheckOverlaps);
        physicalVolumeMap[name] = physVol;  // Store physical volume
    }

    return logicalVolume;
}

/**
 * @brief Creates a G4VSolid object based on the provided solid definition.
 * 
 * @param solidDef The JSON object containing the solid definition.
 * @return A pointer to the created G4VSolid object. Returns nullptr if the shape is not supported.
 */
G4VSolid* DetectorConstruction::CreateSolid(const json& solidDef) {

    G4String shape = solidDef["shape"].get<std::string>();
    if (shape == "tubs") {
        G4double rMin = solidDef["dimensions"]["rMin"].get<double>() * mm;
        G4double rMax = solidDef["dimensions"]["rMax"].get<double>() * mm;
        G4double z = solidDef["dimensions"]["z"].get<double>() * mm;
        G4double startAngle = solidDef["dimensions"]["startAngle"].get<double>() * deg;
        G4double spanningAngle = solidDef["dimensions"]["spanningAngle"].get<double>() * deg;
        return new G4Tubs("Tubs", rMin, rMax, z / 2, startAngle, spanningAngle);
    }
    // Add more shapes as needed (G4Box, G4Sphere, etc.)

    return nullptr;
}

/**
 * @brief Retrieves the logical volume with the specified name.
 * 
 * @param name The name of the logical volume to retrieve.
 * @return A pointer to the logical volume if found, otherwise nullptr.
 */
G4LogicalVolume* DetectorConstruction::GetLogicalVolume(const G4String& name) {
    if (logicalVolumeMap.find(name) != logicalVolumeMap.end()) {
        return logicalVolumeMap[name];
    }
    return nullptr;
}

}  // namespace G4XamsSim