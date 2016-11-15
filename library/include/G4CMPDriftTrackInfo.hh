/***********************************************************************\
 * This software is licensed under the terms of the GNU General Public *
 * License version 3 or later. See G4CMP/LICENSE for the full license. *
\***********************************************************************/

/// \file library/include/G4CMPDriftTrackInfo.hh
/// \brief Definition of the G4CMPDriftTrackInfo class. Used to store
/// auxiliary information that a G4Track can't store, but is necessary for
/// physics processes to know.
///
//
// $Id$
//
// 20161111 Initial commit - R. Agnese

#ifndef G4CMPDriftTrackInfo_hh
#define G4CMPDriftTrackInfo_hh 1

#include "G4CMPVTrackInfo.hh"
#include "G4Allocator.hh"

class G4CMPDriftTrackInfo;

extern G4Allocator<G4CMPDriftTrackInfo> G4CMPDriftTrackInfoAllocator;

class G4CMPDriftTrackInfo: public G4CMPVTrackInfo {
public:
  void Init(const G4LatticePhysical *lat, G4int valIdx) {
    G4CMPVTrackInfo::Init(lat);
    SetValleyIndex(valIdx);
  }

  G4int ValleyIndex() const                                { return valleyIdx; }
  void SetValleyIndex(G4int valIdx);

  virtual void Print() const override;

private:
  G4int valleyIdx;
};

#endif
