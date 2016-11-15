/***********************************************************************\
 * This software is licensed under the terms of the GNU General Public *
 * License version 3 or later. See G4CMP/LICENSE for the full license. *
\***********************************************************************/

/// \file library/include/G4CMPPhononTrackInfo.hh
/// \brief Definition of the G4CMPPhononTrackInfo class. Used to store
/// auxiliary information that a G4Track can't store, but is necessary for
/// physics processes to know.
///
//
// $Id$
//
// 20161111 Initial commit - R. Agnese

#ifndef G4CMPPhononTrackInfo_hh
#define G4CMPPhononTrackInfo_hh 1

#include "G4CMPVTrackInfo.hh"
#include "G4ThreeVector.hh"
#include "G4Allocator.hh"

class G4CMPPhononTrackInfo;

extern G4Allocator<G4CMPPhononTrackInfo> G4CMPPhononTrackInfoAllocator;

class G4CMPPhononTrackInfo: public G4CMPVTrackInfo {
public:
  void Init(const G4LatticePhysical* lat, G4ThreeVector k) {
    G4CMPVTrackInfo::Init(lat);
    waveVec = k;
  }

  void *operator new(size_t) noexcept {
    return static_cast<void*>(G4CMPPhononTrackInfoAllocator.MallocSingle());
  }

  void operator delete(void* info) noexcept {
    G4CMPPhononTrackInfoAllocator.FreeSingle(static_cast<G4CMPPhononTrackInfo*>(info));
  }

  void SetK(G4ThreeVector k)                                    { waveVec = k; }
  void SetWaveVector(G4ThreeVector k)                           { waveVec = k; }
  G4ThreeVector k() const                                    { return waveVec; }
  G4ThreeVector WaveVector() const                           { return waveVec; }

  virtual void Print() const override;

private:
  G4ThreeVector waveVec;
};

#endif
