#include "G4CMPUtils.hh"
#include "G4CMPDriftElectron.hh"
#include "G4CMPDriftHole.hh"
#include "G4LatticePhysical.hh"
#include "G4ParticleDefinition.hh"
#include "G4PhononPolarization.hh"
#include "G4Track.hh"
#include "Randomize.hh"


// Select phonon mode using density of states in material

G4int G4CMP::ChoosePhononPolarization(const G4LatticePhysical* lattice) {
  return ChoosePhononPolarization(lattice->GetLDOS(),
                                  lattice->GetSTDOS(),
                                  lattice->GetFTDOS());
}

G4int G4CMP::ChoosePhononPolarization(G4double Ldos,
                                      G4double STdos, 
                                      G4double FTdos) {
  G4double norm = Ldos + STdos + FTdos;
  G4double cProbST = STdos/norm;
  G4double cProbFT = FTdos/norm + cProbST;

  // NOTE:  Order of selection done to match previous random sequences
  G4double modeMixer = G4UniformRand();
  if (modeMixer<cProbST) return G4PhononPolarization::TransSlow;
  if (modeMixer<cProbFT) return G4PhononPolarization::TransFast;
  return G4PhononPolarization::Long;
}


// Identify G4CMP particle categories
G4bool G4CMP::IsPhonon(const G4Track* track) {
  return (track!=0 && IsPhonon(track->GetParticleDefinition()));
}

G4bool G4CMP::IsPhonon(const G4ParticleDefinition* pd) {
  return (G4PhononPolarization::Get(pd) != G4PhononPolarization::UNKNOWN);
}

G4bool G4CMP::IsElectron(const G4Track* track) {
  return (track!=0 && IsElectron(track->GetParticleDefinition()));
}

G4bool G4CMP::IsElectron(const G4ParticleDefinition* pd) {
  return (pd == G4CMPDriftElectron::Definition());
}

G4bool G4CMP::IsHole(const G4Track* track) {
  return (track!=0 && IsHole(track->GetParticleDefinition()));
}

G4bool G4CMP::IsHole(const G4ParticleDefinition* pd) {
  return (pd == G4CMPDriftHole::Definition());
}

G4bool G4CMP::IsChargeCarrier(const G4Track* track) {
  return (track!=0 && IsChargeCarrier(track->GetParticleDefinition()));
}

G4bool G4CMP::IsChargeCarrier(const G4ParticleDefinition* pd) {
  return (IsElectron(pd) || IsHole(pd));
}
