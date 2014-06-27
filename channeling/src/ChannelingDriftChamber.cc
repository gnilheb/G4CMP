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
/// \file analysis/A01/src/ChannelingDriftChamber.cc
/// \brief Implementation of the ChannelingDriftChamber class
//
// $Id$
// --------------------------------------------------------------
//
#include "ChannelingDriftChamber.hh"
#include "ChannelingDriftHit.hh"
#include "G4HCofThisEvent.hh"
#include "G4TouchableHistory.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4SDManager.hh"
#include "G4Navigator.hh"
#include "G4ios.hh"

ChannelingDriftChamber::ChannelingDriftChamber(G4String name):G4VSensitiveDetector(name)
{
    G4String HCname;
    collectionName.insert(HCname="collection");
    fHCID = -1;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

ChannelingDriftChamber::~ChannelingDriftChamber(){
    ;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void ChannelingDriftChamber::Initialize(G4HCofThisEvent*HCE)
{
    fHitsCollection = new ChannelingDriftHitsCollection(SensitiveDetectorName,collectionName[0]);
    if(fHCID<0){
        fHCID = G4SDManager::GetSDMpointer()->GetCollectionID(fHitsCollection);
    }
    HCE->AddHitsCollection(fHCID,fHitsCollection);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4bool ChannelingDriftChamber::ProcessHits(G4Step*aStep,G4TouchableHistory* /*ROhist*/)
{
    G4double charge = aStep->GetTrack()->GetDefinition()->GetPDGCharge();
//    if(charge!=-1) return true;
//    if(aStep->GetTrack()->GetDefinition()->GetParticleName()!="e-") return true;
    if(charge==0) return true;
  
    G4StepPoint* preStepPoint = aStep->GetPreStepPoint();
    G4StepPoint* postStepPoint = aStep->GetPostStepPoint();

    if(!(postStepPoint->GetStepStatus() == fGeomBoundary)) return true;
    
    G4TouchableHistory* theTouchable = (G4TouchableHistory*)(preStepPoint->GetTouchable());
    G4VPhysicalVolume* thePhysical = theTouchable->GetVolume(0); // mother
    G4int copyNo = thePhysical->GetCopyNo();
    G4ThreeVector worldPos = preStepPoint->GetPosition();
    G4ThreeVector localPos = theTouchable->GetHistory()->GetTopTransform().TransformPoint(worldPos);
    
    ChannelingDriftHit* aHit = new ChannelingDriftHit(copyNo);
    aHit->SetLayerID(copyNo);
    aHit->SetWorldPos(worldPos);
    aHit->SetLocalPos(localPos);
    aHit->SetTime(preStepPoint->GetGlobalTime());
    aHit->SetEnergy(preStepPoint->GetTotalEnergy());
    
    fHitsCollection->insert(aHit);
    return true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void ChannelingDriftChamber::EndOfEvent(G4HCofThisEvent* /*HCE*/)
{
    ;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....