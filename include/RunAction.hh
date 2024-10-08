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
/// \file B1/include/RunAction.hh
/// \brief Definition of the B1::RunAction class

#ifndef _RunAction_h
#define _RunAction_h 1

#include "G4UserEventAction.hh"
#include "G4UserRunAction.hh"
#include "G4Accumulable.hh"
#include "G4AnalysisManager.hh"
#include "globals.hh"
#include "RunActionMessenger.hh"

class G4Run;

/// Run action class

/**
 * @namespace G4Sim
 * @brief Namespace for the G4Sim library.
/*/
namespace G4Sim
{

class EventAction;
class RunActionMessenger;

/**
 * @class RunAction
 * @brief User-defined run action class.
 *
 * This class inherits from G4UserRunAction and is responsible for defining the actions to be taken at the beginning and end of a run.
 * It also provides methods for initializing and defining event ntuples, as well as setting the output file name.
 *
 * @note The default output file name is "G4XamsSim.root".
 */
class RunAction : public G4UserRunAction
{
  public:
    RunAction(EventAction *eventAction);
    ~RunAction();

    void BeginOfRunAction(const G4Run*) override;
    void   EndOfRunAction(const G4Run*) override;

    void InitializeNtuples();
    void DefineEventNtuple();
    void SetOutputFileName(G4String value) { fOutputFileName = value; }

  private:
    EventAction* fEventAction = nullptr;
    RunActionMessenger* fMessenger;

    int eventNtupleId = -1;

    G4String fOutputFileName = "G4XamsSim.root";
};

} // namespace G4Sim

#endif

