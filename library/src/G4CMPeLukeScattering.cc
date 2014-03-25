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
/// \file library/src/G4eLukeScattering.cc
/// \brief Implementation of the G4eLukeScattering class
//
// $Id$
//
// 20140325  Move time-step calculation to G4CMPProcessUtils

#include "G4CMPeLukeScattering.hh"
#include "G4CMPDriftElectron.hh"
#include "G4CMPValleyTrackMap.hh"
#include "G4LatticeManager.hh"
#include "G4LatticePhysical.hh"
#include "G4PhysicalConstants.hh"
#include "G4RandomDirection.hh"
#include "G4Step.hh"
#include "G4SystemOfUnits.hh"
#include "G4VParticleChange.hh"
#include "Randomize.hh"
#include <fstream>
#include <iostream>


G4CMPeLukeScattering::G4CMPeLukeScattering(G4VProcess* sLim)
 : G4CMPVDriftProcess("eLukeScattering"), stepLimiter(sLim) {
  if (verboseLevel > 1) {
    G4cout << "G4CMPeLukeScattering::Constructor: ksound_e = "
	   << ksound_e*m << " /m" << G4endl
	   << GetProcessName() << " is created." << G4endl;
  }
}

G4CMPeLukeScattering::~G4CMPeLukeScattering() {;}


G4double G4CMPeLukeScattering::GetMeanFreePath(const G4Track& aTrack,
					       G4double,
					       G4ForceCondition* condition) {
  *condition = Forced;
  
  G4RotationMatrix trix = GetValley(aTrack);
  valleyToNormal= G4AffineTransform(trix);
  normalToValley= G4AffineTransform(trix).Inverse();    

  G4ThreeVector k = aTrack.GetMomentum()/hbarc;
  mInv = theLattice->GetMInvTensor();
  G4ThreeVector v = ((trix.inverse()*mInv*trix) * k) * hbar_Planck;
  G4cout << "v = " << v.mag()/m*s << G4endl;
  G4ThreeVector k_valley = normalToValley.TransformPoint(k);
  G4ThreeVector k_HV = theLattice->GetSqrtInvTensor() * k_valley;
  G4double kmag = k_HV.mag();
  
  if (kmag<=ksound_e) return DBL_MAX;
 
  // Time step corresponding to Mach number (avg. time between radiations)
  G4double dtau = ChargeCarrierTimeStep(kmag/ksound_e, l0_e);
  
  G4double mfp = dtau * v.mag();
  G4cout << "MFP = " << mfp/m << G4endl;
  return mfp;
}

G4VParticleChange* G4CMPeLukeScattering::PostStepDoIt(const G4Track& aTrack,
						      const G4Step& aStep) {
  aParticleChange.Initialize(aTrack); 
  G4StepPoint* postStepPoint = aStep.GetPostStepPoint();
  
  G4ThreeVector k = postStepPoint->GetMomentum()/hbarc;
  G4ThreeVector k_valley = normalToValley.TransformPoint(k);
  G4ThreeVector k_HV = theLattice->GetSqrtInvTensor() * k_valley;
  G4double kmag = k_HV.mag();
  
  //Do nothing other than re-calculate mfp when step limit reached or leaving
  //volume
  if ( (postStepPoint->GetProcessDefinedStep()==stepLimiter)
       || (postStepPoint->GetStepStatus()==fGeomBoundary)
       || (kmag <= ksound_e) ) {
    return &aParticleChange;
  }

  G4double theta_phonon = MakeTheta(kmag, ksound_e);
  G4double theta_charge = (theta_phonon == 0.) ? 0. : 
    acos( (kmag*kmag - 2*ksound_e*(kmag*cos(theta_phonon)
				 - ksound_e)
	   - 2 * (kmag*cos(theta_phonon) - ksound_e)
	   * (kmag*cos(theta_phonon) - ksound_e) )/
	  kmag/ (sqrt(kmag*kmag - 4*ksound_e
		      * (kmag*cos(theta_phonon) - ksound_e) ) ) );
  
  G4double q = 2*(kmag*cos(theta_phonon)-ksound_e);
  //k_HV.setMag(sqrt(k_HV.mag2() + q*q - 2*kmag*q*cos(theta_phonon)));
  k_HV.setMag(sqrt(kmag*kmag-2*mc_e*q*velLong/hbar_Planck));
  G4ThreeVector kdir = k_HV.unit();
  k_HV.rotate(kdir.orthogonal(), theta_charge);
  
  G4double phi_charge =  G4UniformRand()*2*pi;
  k_HV.rotate(kdir, phi_charge);
  
  G4ThreeVector p_new = theLattice->GetSqrtTensor() * (hbar_Planck*k_HV);
  valleyToNormal.ApplyAxisTransform(p_new);

  // FIXME:  Need to generate actual phonon!
  
  //G4ThreeVector phononq = q*k.unit().rotate(k.unit(), theta_phonon);
  //   std::ofstream charge("theta_charge", std::ios::app);
  //std::ofstream phonon("theta_phonon", std::ios::app);
  //   charge << theta_charge << G4endl;
  //phonon << phononq.getZ()/phononq.mag()<< G4endl;
  //   charge.close();
  //phonon.close();

  //std::ofstream espec("energy_spectrum_phonon", std::ios::app);
  //espec << (k_HV.mag2()*hbar_Planck/2/mc)/eV*1000<< G4endl;
  //espec.close();

  G4double Enew = p_new.mag2()/2/mc_e;
  aParticleChange.ProposeMomentumDirection(p_new.unit());
  aParticleChange.ProposeEnergy(Enew);
  aParticleChange.ProposeNonIonizingEnergyDeposit(aTrack.GetKineticEnergy()-Enew);

  ResetNumberOfInteractionLengthLeft();
  return &aParticleChange;
}


G4double G4CMPeLukeScattering::MakeTheta(G4double& k, G4double& ks) {
  //Analytical method to compute theta
  G4double u = G4UniformRand();

  G4double base = -(u-1)+3*(u-1)*(ks/k)-3*(u-1)*(ks/k)*(ks/k)+(u-1)*(ks/k)*(ks/k)*(ks/k);
  if(base < 0.0) return 0;

  G4double operand = ks/k+pow(base, 1.0/3.0);   
  if(operand > 1.0) operand=1.0;

  G4double theta = acos(operand);
  return theta;
}

G4double G4CMPeLukeScattering::MakePhi(G4double& k,G4double& ks, G4double& theta) {
  G4double phi=acos((k*k - 2*ks*(k*cos(theta)-ks)-2*(k*cos(theta)-ks)*(k*cos(theta)-ks))/(k*sqrt(k*k-4*ks*(k*cos(theta)-ks))));
  return phi;
}

G4bool G4CMPeLukeScattering::IsApplicable(const G4ParticleDefinition& aPD) {
  return (&aPD==G4CMPDriftElectron::Definition());
}
