// This file is part of HemeLB and is Copyright (C)
// the HemeLB team and/or their institutions, as detailed in the
// file AUTHORS. This software is provided under the terms of the
// license in the file LICENSE.

#ifndef HEMELBSETUPTOOL_SITE_H
#define HEMELBSETUPTOOL_SITE_H

#include <vector>
#include "Index.h"
#include "io/formats/geometry.h"
#include "Debug.h"
// shortcut to geometry class
using hemelb::io::formats::geometry;

// class Iolet;
class Domain;
class Block;

class LaterNeighbourIterator;
class NeighbourIterator;

struct LinkData {
  geometry::CutType Type;
  float Distance;
  unsigned int IoletId;
  float DistanceInVoxels;
  Vector WallNormalAtWallCut;
  inline LinkData()
      : Type(geometry::CutType::NONE),
        Distance(0.),
        DistanceInVoxels(0.),
        WallNormalAtWallCut(0){};
};

// A single lattice site
class Site {
 public:
  Site(Block& block, Index& index);
  Site(Block& block, int i, int j, int k);
  bool IsFluidKnown;
  bool IsFluid;
  bool IsHalo;
  std::vector<LinkData> Links;
  Vector Position;
  bool WallNormalAvailable;  ///< Whether an approximation of the wall normal is
                             ///< available
  Vector WallNormal;         ///< Approximation of the wall normal on this site

  inline void CreateLinksVector() {
    Links.resize(geometry::NumberOfDisplacements);
  }

  LaterNeighbourIterator begin();
  LaterNeighbourIterator end();
  NeighbourIterator beginall();
  NeighbourIterator endall();
  inline const Index& GetIndex() const { return this->index; }
  inline const Block& GetBlock() const { return this->block; }
  inline Block& GetBlock() { return this->block; }
  //inline const Index GetRelativeIndex() const { return index((this->index[0] % block.size)+1, (this->index[1] % block.size)+1, (this->index[2] % block.size)+1); }
  const Index GetDomainBlockCount();
  const int GetDomainBlockSize();

 protected:
  Block& block;
  Index index;
  friend class NeighbourIteratorBase;
  friend class LaterNeighbourIterator;
};

// Base for iterating over neighbours
class NeighbourIteratorBase
    : public std::iterator<std::forward_iterator_tag, Site> {
 public:
  NeighbourIteratorBase(Site& site, unsigned int startpos = 0);
  NeighbourIteratorBase(const NeighbourIteratorBase& other);

  NeighbourIteratorBase& operator=(const NeighbourIteratorBase& other);
  NeighbourIteratorBase& operator++();
  bool operator==(const NeighbourIteratorBase& other) const;
  bool operator!=(const NeighbourIteratorBase& other) const;
  reference operator*();
  pointer operator->();

  virtual unsigned int GetNeighbourIndex();

 protected:
  Site* site;
  Domain* domain;
  unsigned int i;

  void AdvanceToValid();
  bool IsCurrentInDomain();
  virtual bool IsCurrentValid() = 0;
  Index GetVector();
};

// Iterator for getting all the later (i.e. further on in memory) neighbouring
// sites of a given site
class LaterNeighbourIterator : public NeighbourIteratorBase {
 public:
  inline LaterNeighbourIterator(Site& site, unsigned int startpos = 0)
      : NeighbourIteratorBase(site, startpos) {
    this->AdvanceToValid();
  }

 protected:
  bool IsCurrentValid();
};

// Iterator for getting all the later (i.e. further on in memory) neighbouring
// sites of a given site
class NeighbourIterator : public NeighbourIteratorBase {
 public:
  inline NeighbourIterator(Site& site, unsigned int startpos = 0)
      : NeighbourIteratorBase(site, startpos) {
    this->AdvanceToValid();
  }

 protected:
  bool IsCurrentValid();
};
#endif  // HEMELBSETUPTOOL_SITE_H
