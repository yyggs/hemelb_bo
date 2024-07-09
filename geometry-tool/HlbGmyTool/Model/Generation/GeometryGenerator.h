// This file is part of HemeLB and is Copyright (C)
// the HemeLB team and/or their institutions, as detailed in the
// file AUTHORS. This software is provided under the terms of the
// license in the file LICENSE.

#ifndef HEMELBSETUPTOOL_GEOMETRYGENERATOR_H
#define HEMELBSETUPTOOL_GEOMETRYGENERATOR_H

#include <string>
#include <vector>

#include "GenerationError.h"
#include "Iolet.h"
#include "Debug.h"

class GeometryWriter;
class Site;
class BlockWriter;
class Block;
class Domain;
class BlockIterator;

class GeometryGenerator {
 public:
  GeometryGenerator();
  virtual ~GeometryGenerator();
  void Execute(bool skipNonIntersectingBlocks);

  inline std::string GetOutputGeometryFile(void) {
    return this->OutputGeometryFile;
  }
  inline void SetOutputGeometryFile(std::string val) {
    this->OutputGeometryFile = val;
  }

  inline std::vector<Iolet>& GetIolets() { return this->Iolets; }
  inline std::vector<Iolet> const& GetIolets() const { return this->Iolets; }
  inline void SetIolets(std::vector<Iolet> iv) { this->Iolets = iv; }

  inline void SetOriginWorking(double x, double y, double z) {
    this->OriginWorking[0] = x;
    this->OriginWorking[1] = y;
    this->OriginWorking[2] = z;
  }

  inline void SetSiteCounts(unsigned x, unsigned y, unsigned z) {
    this->SiteCounts[0] = x;
    this->SiteCounts[1] = y;
    this->SiteCounts[2] = z;
  }

  /**
   * This method implements the algorithm used to approximate the wall normal at
   * a given fluid site. This is done based on the normal of the triangles
   * intersected by each lattice link and the distance to those intersections.
   *
   * Current implementation does a weighted sum of the wall normals. The weights
   * are the reciprocal of cut distances along each link.
   *
   * @param site Site object with the data required by the algorithm.
   */
  void ComputeAveragedNormal(Site& site) const;

 protected:
  void ProcessBlock(BlockIterator blockIt, GeometryWriter& writer, 
        std::ofstream& file, bool skipNonIntersectingBlocks);
  virtual void ComputeBounds(double[]) const = 0;
  virtual void PreExecute(void);
  virtual void ClassifySite(Site& site) = 0;
  virtual void ClassifyStartingSite(Site& originSite, Site& site) = 0;
  // virtual void CreateCGALPolygon(void);
  void WriteSolidSite(BlockWriter& blockWriter, Site& site);
  void WriteFluidSite(BlockWriter& blockWriter, Site& site);
  // Members set from outside to initialise
  double OriginWorking[3];
  unsigned SiteCounts[3];
  std::string OutputGeometryFile;
  std::vector<Iolet> Iolets;
  virtual int BlockInsideOrOutsideSurface(const Block& block) = 0;
  void ComputeStartingSite(Site& startSite);
};

#endif  // HEMELBSETUPTOOL_GEOMETRYGENERATOR_H
