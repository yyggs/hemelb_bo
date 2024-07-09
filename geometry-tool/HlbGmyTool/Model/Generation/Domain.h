// This file is part of HemeLB and is Copyright (C)
// the HemeLB team and/or their institutions, as detailed in the
// file AUTHORS. This software is provided under the terms of the
// license in the file LICENSE.

#ifndef HEMELBSETUPTOOL_DOMAIN_H
#define HEMELBSETUPTOOL_DOMAIN_H

#include <vector>
#include <atomic>

#include "BlockWriter.h"
#include "Index.h"
#include "Debug.h"

class Block;
class Site;
class BlockIterator;

// Note that the "working" units for this are voxels.

class Domain {
 protected:
  Index BlockCounts;
  Index SiteCounts;
  Vector OriginWorking;
  int BlockSize;

  std::vector<Block*> blocks;
  std::vector<BlockWriter*> blockWriters;
  std::atomic<bool>* blockready;
  int BlockWritingNum;

  friend class BlockIterator;
  friend class NeighbourIteratorBase;

 public:
  using iterator = BlockIterator;
  /*
   * C'tor
   * SurfaceBounds - bounds of the surface, in standard VTK order
   * (x_min, x_max, y_min, y_max, z_min, z_max), in voxels.
   * BlockSize - number of sites along one dimension.
   */
  Domain(double OriginWorking[3],
         unsigned SiteCounts[3],
         unsigned BlockSize = 8);

  Vector CalcPositionWorkingFromIndex(const Index& index) const;
  Block& GetBlock(const Index& index);
  Site& GetSite(const Index& index);
  BlockIterator begin();
  BlockIterator end();

  inline int GetBlockSize() const { return BlockSize; }
  inline void SetBlockSize(int val) { BlockSize = val; }

  inline Index const& GetBlockCounts() const { return BlockCounts; }
  inline void SetBlockCounts(Index const& val) { BlockCounts = val; }

  inline Index const& GetSiteCounts() const { return SiteCounts; }
  inline void SetBlockWriter(Index const& index, BlockWriter* writer) {
    int blocknumber = index[2] + index[1] * BlockCounts[2] + index[0] * BlockCounts[1] * BlockCounts[2];
    this->blockWriters[blocknumber] = writer;
    this->blockready[blocknumber] = true;
  }
  inline bool CheckBlockReady() {
    return this->blockready[this->BlockWritingNum];
  }
  inline BlockWriter* GetBlockWriter() {
    return this->blockWriters[this->BlockWritingNum++];
  }
  inline bool CheckWritingDone() {
    if(this->BlockWritingNum >= this->BlockCounts[0] * this->BlockCounts[1] * this->BlockCounts[2])
      return true;
    return false;
  }

  /*
   * These TranslateIndex member functions translate between 3d and 1a
   * indices, i.e.
   * 		ijk = (i * ny + j) * nz + k
   * and the inverse. The direction is inferred from the type of the first
   * argument.
   */

  // 1d => 3d
  inline Index* TranslateIndex(const unsigned int i) {
    Index* bInd = new Index();
    this->TranslateIndex(i, *bInd);
    return bInd;
  }
  // 1d => 3d, putting the answer in an existing Index
  inline void TranslateIndex(const unsigned int k, Index& ans) {
    ans[2] = k % this->BlockCounts[2];
    int j = k / this->BlockCounts[2];

    ans[1] = j % this->BlockCounts[1];
    int i = j / this->BlockCounts[1];

    ans[0] = i % this->BlockCounts[0];
#ifdef CHECK_BOUNDS
    if (i / this->BlockCounts[0])
      throw IndexError;
#endif
  }

  // 3d => 1d
  inline int TranslateIndex(const Index& ijk) {
    return (ijk[0] * this->BlockCounts[1] + ijk[1]) * this->BlockCounts[2] +
           ijk[2];
  }
  inline int TranslateIndex(const unsigned int& i,
                            const unsigned int& j,
                            const unsigned int& k) {
    return (i * this->BlockCounts[1] + j) * this->BlockCounts[2] + k;
  }
};

class BlockIterator : public std::iterator<std::forward_iterator_tag, Block> {
 public:
  BlockIterator();
  BlockIterator(Domain& dom);
  BlockIterator(Domain& dom, const Index& start);
  BlockIterator(const BlockIterator& other);
  //	~BlockIterator();

  BlockIterator& operator=(const BlockIterator& other);
  BlockIterator& operator++();
  bool operator==(const BlockIterator& other) const;
  bool operator!=(const BlockIterator& other) const;
  reference operator*();
  pointer operator->();

 protected:
  Domain* domain;
  Index current;
  Index maxima;
};
#endif  // HEMELBSETUPTOOL_DOMAIN_H
