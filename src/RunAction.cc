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
/// \file B1/src/RunAction.cc
/// \brief Implementation of the B1::RunAction class

#include "RunAction.hh"
#include "EventAction.hh"	
#include "PrimaryGeneratorAction.hh"
#include "DetectorConstruction.hh"
// #include "Run.hh"

#include "G4RunManager.hh"
#include "G4Run.hh"
#include "G4AccumulableManager.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

#include "G4AnalysisManager.hh"
#include "G4Gamma.hh"
#include "G4PhysicalConstants.hh"

#include <cmath>

/**
 * @namespace G4Sim
 * @brief Namespace for the G4Sim library.
/*/
namespace G4Sim{
/**
 * @file RunAction.cc
 * @brief Implementation of the RunAction class.
 *
 * The RunAction class is responsible for managing actions that occur during a run of the simulation.
 * It initializes and defines the analysis manager, creates and fills ntuples for event data, cross-section data,
 * and differential cross-section data, and performs actions at the beginning and end of a run.
 */


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

/**
 * @brief Constructs a RunAction object with the given eventAction.
 * 
 * @param eventAction The EventAction object associated with this RunAction.
 */
RunAction::RunAction(EventAction* eventAction)
  : fEventAction(eventAction)
{
  // set printing event number per each event
  G4RunManager::GetRunManager()->SetPrintProgress(1000);

  // Create the generic analysis manager
  auto analysisManager = G4AnalysisManager::Instance();

  fMessenger = new RunActionMessenger(this);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

/**
 * @brief This function is called at the beginning of each run.
 * 
 * It retrieves the initial energy from the primary generator action and prints it to the console.
 * 
 * It also initializes the analysis manager and ntuples.
 * 
 * @param run Pointer to the G4Run object representing the current run.
 */
void RunAction::BeginOfRunAction(const G4Run*)
{

  // Get the initial energy from the primary generator action
  const PrimaryGeneratorAction* primaryGeneratorAction = static_cast<const PrimaryGeneratorAction*>(
    G4RunManager::GetRunManager()->GetUserPrimaryGeneratorAction());

  G4cout << "Runaction::BeginOfRunAction: E0 = " << primaryGeneratorAction->GetInitialEnergy() / keV << " keV" << G4endl;

  // initialize the analysis manager and ntuples
  InitializeNtuples();

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
RunAction::~RunAction()
{
  delete fMessenger;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

/**
 * @brief Initializes the analysis manager and creates event data ntuple.
 * 
 * This function initializes the analysis manager and sets the default file type to "root".
 * It opens the output file specified by `fOutputFileName`.
 * The verbose level is set to 1.
 * Ntuple merging is enabled.
 * Finally, it calls the `DefineEventNtuple()` function to create the event data ntuple.
 * 
 * @note This function should be called only in the master thread.
 */
void RunAction::InitializeNtuples(){

  // Get analysis manager
  auto analysisManager = G4AnalysisManager::Instance();
  if (G4Threading::IsMasterThread()) {
    analysisManager->SetDefaultFileType("root");
    analysisManager->OpenFile(fOutputFileName);
    //  
    analysisManager->SetVerboseLevel(1);
    // Default settings
    analysisManager->SetNtupleMerging(true);
    // Creating event data ntuple
    DefineEventNtuple();
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

/**
 * @brief Defines the event ntuple for data analysis.
 * 
 * This function creates an event ntuple using the G4AnalysisManager class. The ntuple contains columns for storing energy deposition (Edep), x-coordinate (xh), and y-coordinate (yh) of each event. The ntuple is finished and assigned an ID.
 */
void RunAction::DefineEventNtuple(){
  // Creating ntuple
  auto analysisManager = G4AnalysisManager::Instance();

  G4cout << "RunAction::BeginOfRunAction: Creating event data ntuple" << G4endl;

  eventNtupleId = analysisManager->CreateNtuple("ev", "G4XamsSim ntuple");
  analysisManager->CreateNtupleDColumn(eventNtupleId, "ev");   // column Id = 0
  analysisManager->CreateNtupleDColumn(eventNtupleId, "w");    // column Id = 1
  analysisManager->CreateNtupleDColumn(eventNtupleId, "type"); // column Id = 2
  analysisManager->CreateNtupleDColumn(eventNtupleId, "xp");   // column Id = 3
  analysisManager->CreateNtupleDColumn(eventNtupleId, "yp");   // column Id = 4
  analysisManager->CreateNtupleDColumn(eventNtupleId, "zp");   // column Id = 5
  analysisManager->CreateNtupleDColumn(eventNtupleId, "eh", fEventAction->GetE()); 
  analysisManager->CreateNtupleDColumn(eventNtupleId, "xh", fEventAction->GetX()); 
  analysisManager->CreateNtupleDColumn(eventNtupleId, "yh", fEventAction->GetY()); 
  analysisManager->CreateNtupleDColumn(eventNtupleId, "zh", fEventAction->GetZ()); 
  analysisManager->CreateNtupleDColumn(eventNtupleId, "wh", fEventAction->GetW()); 
  analysisManager->CreateNtupleIColumn(eventNtupleId, "id", fEventAction->GetID()); 
  analysisManager->CreateNtupleDColumn(eventNtupleId, "edet", fEventAction->GetEdet());
  analysisManager->CreateNtupleIColumn(eventNtupleId, "ndet", fEventAction->GetNdet());
  analysisManager->CreateNtupleIColumn(eventNtupleId, "nphot", fEventAction->GetNphot());
  analysisManager->CreateNtupleIColumn(eventNtupleId, "ncomp", fEventAction->GetNcomp());

  analysisManager->FinishNtuple(eventNtupleId);
  G4cout <<"RunAction::BeginOfRunAction: Event data ntuple created. ID = "<< eventNtupleId << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

/**
 * @brief This function is called at the end of a run.
 * It saves histograms and ntuple if the current thread is the master thread.
 * 
 * @param run Pointer to the G4Run object representing the current run.
 */
void RunAction::EndOfRunAction(const G4Run* run)
{

  // save histograms & ntuple
  //
  if (G4Threading::IsMasterThread()) {
      auto analysisManager = G4AnalysisManager::Instance();
      analysisManager->Write();
      analysisManager->CloseFile();
  }

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

} // namespace G4Sim
