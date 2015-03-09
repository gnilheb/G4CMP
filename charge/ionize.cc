#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4Version.hh"

#ifdef G4VIS_USE
#include "G4VisExecutive.hh"
#endif

#ifdef G4UI_USE
#include "G4UIExecutive.hh"
#endif

#include "ChargeActionInitialization.hh"
#include "ChargeDetectorConstruction.hh"
#include "ChargePrimaryGeneratorAction.hh"
#include "G4CMPConfigManager.hh"
#include "G4CMPPhysics.hh"
#include "G4CMPStackingAction.hh"
#include "FTFP_BERT.hh"

int main(int argc,char** argv) {
  // Construct the run manager
  //
  G4RunManager* runManager = new G4RunManager;
  
  // Set mandatory initialization classes
  //
  ChargeDetectorConstruction* detector = new ChargeDetectorConstruction;
  runManager->SetUserInitialization(detector);
  
  FTFP_BERT* physics = new FTFP_BERT;
  physics->RegisterPhysics(new G4CMPPhysics);
  physics->SetCuts();
  runManager->SetUserInitialization(physics);
  
  // Set user action classes (different for Geant4 10.0)
  //
#if (G4VERSION_NUMBER >= 1000)
  runManager->SetUserInitialization(new ChargeActionInitialization);
#else
  runManager->SetUserAction(new G4CMPStackingAction);
  runManager->SetUserAction(new ChargePrimaryGeneratorAction);
#endif
  
  // Create G4CMP configuration manager to ensure macro commands exist
  G4CMPConfigManager::GetVerboseLevel();
  
  //FET* fetsim = new FET(runManager);
  
#ifdef G4VIS_USE
  // Visualization manager
  //
  G4VisManager* visManager = new G4VisExecutive("quiet");
  visManager->Initialize();
#endif
  
  // Get the pointer to the User Interface manager
  //
  G4UImanager* UImanager = G4UImanager::GetUIpointer();  
  
  if (argc==1) {   // Define UI session for interactive mode
#ifdef G4UI_USE
    G4UIExecutive * ui = new G4UIExecutive(argc,argv);
    ui->SessionStart();
    delete ui;
#endif
  } else {           // Batch mode
    G4String command = "/control/execute ";
    G4String fileName = argv[1];
    UImanager->ApplyCommand(command+fileName);
  }
  
  //G4cout << runManager->GetCurrentRun()->GetEventVector()->size() << G4endl;
  // FET* fet = new FET(runManager->GetCurrentRun());
  // delete fet;
#ifdef G4VIS_USE
  delete visManager;
#endif
  delete runManager;
  
  return 0;
}
