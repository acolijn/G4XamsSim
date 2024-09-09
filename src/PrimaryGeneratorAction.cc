#include "PrimaryGeneratorAction.hh"

#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4Box.hh"
#include "G4RunManager.hh"
#include "G4GeneralParticleSource.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"

///namespace G4FastSim
///{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

/**
 * @brief Constructor for the PrimaryGeneratorAction class.
 * 
 * This constructor initializes the PrimaryGeneratorAction object by creating a new instance of the G4GeneralParticleSource class.
 */
PrimaryGeneratorAction::PrimaryGeneratorAction()
{
  fParticleGun  = new G4GeneralParticleSource();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  delete fParticleGun;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

/**
 * @brief This function is called at the beginning of an event to generate primary particles.
 * 
 * @param anEvent Pointer to the G4Event object representing the current event.
 */
void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  //this function is called at the begining of event
  fParticleGun->GeneratePrimaryVertex(anEvent);
}

G4double PrimaryGeneratorAction::GetInitialEnergy() const {
    return fInitialEnergy;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

///}


