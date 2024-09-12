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

using namespace G4Sim;
using json = nlohmann::json;

/**
 * @namespace G4Sim
 * @brief Namespace for the G4Sim library.
/*/
namespace G4Sim {

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
    // construct materials
    fMaterials = new Materials(matFileName);
    fMaterials->DefineMaterials();

    // construct geometry
    G4cout << "DetectorConstruction::Construct: Loading geometry from JSON file: " << geoFileName << G4endl;
    LoadGeometryFromJson(geoFileName);
    G4cout << "DetectorConstruction::Construct: Geometry loaded successfully!" << G4endl;

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
    geoFileName = fileName;
    G4cout << "DetectorConstruction::SetGeometryFilename: JSON file name set to: " << geoFileName << G4endl;
}

void DetectorConstruction::SetMaterialFileName(const std::string& fileName) {
    matFileName = fileName;
    G4cout << "DetectorConstruction::SetMaterialFilename: JSON file name set to: " << matFileName << G4endl;
}   

/**
 * Loads the geometry from a JSON file.
 * 
 * @param jsonFileName The path to the JSON file containing the geometry information.
 */
void DetectorConstruction::LoadGeometryFromJson(const std::string& geoFileName) {

    std::ifstream inputFile(geoFileName);
    if (!inputFile.is_open()) {
        G4cerr << "DetectorConstruction::LoadGeometryFromJson: Error: Could not open geometry JSON file: " << geoFileName << G4endl;
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
        // Construct the logical volume
        G4LogicalVolume* logVol = ConstructVolume(volume);
        // Store the logical volume in the map
        if (logVol) {
            logicalVolumeMap[volume["name"]] = logVol;  // Store logical volume

            // If the volume is marked as active, make it a sensitive detector
            G4String name = volume["name"].get<std::string>();
            if (volume.contains("active") && volume["active"].get<bool>()) {
                G4cout << "Making volume sensitive: " << name << G4endl;
                MakeVolumeSensitive(name, name + "Collection");

                G4double spatialThreshold = 10.0 * mm;  // default
                G4double timeThreshold = 100.0 * ns;    // default

                if (volume.contains("clustering")) {
                    if (volume["clustering"].contains("spatialThreshold")) {
                        spatialThreshold = volume["clustering"]["spatialThreshold"].get<double>() * mm;
                    }
                    if (volume["clustering"].contains("timeThreshold")) {
                        timeThreshold = volume["clustering"]["timeThreshold"].get<double>() * ns;
                    }
                }

                // Store the thresholds in the map
                fClusteringParameters[name] = std::make_pair(spatialThreshold, timeThreshold);
            }
        
            // create the Physical Volume
            //G4VPhysicalVolume* physVol = new G4PVPlacement(nullptr, position, logicalVolume, name, parentVolume, false, 0, fCheckOverlaps);
            G4VPhysicalVolume* physicalVolume = PlaceVolume(volume, logVol);
            physicalVolumeMap[name] = physicalVolume;  // Store physical volume
        }
    }

    G4cout << "Setting clustering parameters in EventAction." << G4endl;
    EventAction::SetClusteringParameters(fClusteringParameters);
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
    auto* sensitiveDetector = new G4Sim::SensitiveDetector(volumeName, collectionName);
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

    G4cout << "DetectorConstruction::ConstructVolume: Constructing volume: " << name << G4endl;
    G4cout << "DetectorConstruction::ConstructVolume: Material: " << material->GetName() << G4endl;  

    // Handle compound shapes (e.g., G4UnionSolid)

    // if shape is 'union'
    if (volumeDef["shape"].get<std::string>() == "union"){

    //if (volumeDef.contains("components")) {
        G4VSolid* unionSolid = nullptr;
        for (const auto& component : volumeDef["components"]) {
            // create the solid
            G4VSolid* solid = CreateSolid(component);
            // its relative position
            G4ThreeVector position(component["placement"]["x"].get<double>() * mm,
                                   component["placement"]["y"].get<double>() * mm,
                                   component["placement"]["z"].get<double>() * mm);
            // its rotation (if exists)
            G4RotationMatrix* rotation = GetRotationMatrix(component);
            // Call the function that handles rotation
            if (!unionSolid) {
                unionSolid = solid;  // First component
            } else {
                unionSolid = new G4UnionSolid(name, unionSolid, solid, rotation, position);
            }
        }
        logicalVolume = new G4LogicalVolume(unionSolid, material, name);
    } else {
        G4VSolid* solid = CreateSolid(volumeDef);
        logicalVolume = new G4LogicalVolume(solid, material, name);
    }
 
    return logicalVolume;
}

/**
 * @brief Places a volume inside its parent volume based on the provided JSON definition.
 *
 * This function reads the volume definition from a JSON object and places the volume
 * inside its parent volume. If a parent volume is specified in the JSON definition, 
 * it retrieves the parent volume by name. If no parent is specified, it defaults to 
 * placing the volume inside the world volume. The function also handles the position 
 * and optional rotation of the volume.
 *
 * @param volumeDef A JSON object containing the volume definition, including its name, 
 *                  parent volume (optional), and placement information (position and rotation).
 * @param logicalVolume A pointer to the logical volume to be placed.
 * @return A pointer to the placed physical volume, or nullptr if an error occurs.
 */
G4VPhysicalVolume* DetectorConstruction::PlaceVolume(const json& volumeDef, G4LogicalVolume* logicalVolume) {

    G4String name = volumeDef["name"].get<std::string>();
    G4VPhysicalVolume* physicalVolume = nullptr;

   // Place volume inside its parent
    if (logicalVolume) {
        G4LogicalVolume* parentVolume = fWorldLogical;  // Default to world

        if (volumeDef.contains("parent")) {
            G4String parentName = volumeDef["parent"].get<std::string>();
            parentVolume = GetLogicalVolume(parentName);
            if (!parentVolume) {
                G4cerr << "Error: Parent volume " << parentName << " not found!" << G4endl;
                exit(-1);
            }
        }

        G4ThreeVector position(volumeDef["placement"]["x"].get<double>() * mm, 
                               volumeDef["placement"]["y"].get<double>() * mm, 
                               volumeDef["placement"]["z"].get<double>() * mm);

        // Extract rotation (if exists)
        G4RotationMatrix* rotation = GetRotationMatrix(volumeDef);

        // place the volume
        physicalVolume = new G4PVPlacement(
            rotation,  // Rotation matrix (can be nullptr)
            position,        // Position vector
            logicalVolume,   // Logical volume to place
            name,      // Name of the volume
            parentVolume,    // Parent logical volume
            false,           // No boolean operation
            0,               // Copy number
            fCheckOverlaps   // Overlap checking
        );
    }

    return physicalVolume;
}

/**
 * @brief Creates a G4RotationMatrix based on the rotation parameters provided in the JSON object.
 *
 * This function extracts the rotation parameters (if they exist) from the given JSON object and 
 * creates a G4RotationMatrix using the specified Euler angles (x, y, z). The angles are expected 
 * to be in degrees and will be converted to radians internally.
 *
 * @param volumeDef A JSON object containing the volume definition, including placement and rotation parameters.
 * @return A pointer to a G4RotationMatrix object initialized with the specified rotation angles, or nullptr if no rotation is specified.
 */
G4RotationMatrix* DetectorConstruction::GetRotationMatrix(const json& volumeDef){

    G4RotationMatrix* rotationMatrix = nullptr;

    // Extract rotation (if exists)
    json rotationJson;
    if (volumeDef["placement"].contains("rotation")) {
        rotationJson = volumeDef["placement"]["rotation"];
    }

    if (rotationJson.contains("x") || rotationJson.contains("y") || rotationJson.contains("z")) {
        G4double rotationX = 0.0;
        G4double rotationY = 0.0;
        G4double rotationZ = 0.0;

        if (rotationJson.contains("x")) {
            rotationX = rotationJson["x"].get<double>() * deg;
        }
        if (rotationJson.contains("y")) {
            rotationY = rotationJson["y"].get<double>() * deg;
        }
        if (rotationJson.contains("z")) {
            rotationZ = rotationJson["z"].get<double>() * deg;
        }

        // Create the rotation matrix with the Euler angles
        rotationMatrix = new G4RotationMatrix();
        rotationMatrix->rotateX(rotationX);
        rotationMatrix->rotateY(rotationY);
        rotationMatrix->rotateZ(rotationZ);
    }

    return rotationMatrix;
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
    } else if (shape == "box"){
        G4double x = solidDef["dimensions"]["x"].get<double>() * mm;
        G4double y = solidDef["dimensions"]["y"].get<double>() * mm;
        G4double z = solidDef["dimensions"]["z"].get<double>() * mm;
        return new G4Box("Box", x / 2, y / 2, z / 2);
    } else {
        G4cerr << "Error: Unsupported shape: " << shape << G4endl;
        exit(-1);
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