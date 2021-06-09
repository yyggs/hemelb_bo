// This file is part of HemeLB and is Copyright (C)
// the HemeLB team and/or their institutions, as detailed in the
// file AUTHORS. This software is provided under the terms of the
// license in the file LICENSE.

#include "tests/helpers/FourCubeBasedTestFixture.h"
#include "tests/helpers/OneInOneOutSimConfig.h"


namespace hemelb
{
  namespace tests
  {
    namespace helpers
    {
      FourCubeBasedTestFixtureBase::FourCubeBasedTestFixtureBase(int cubesize) :
	initParams(), cubeSize(cubesize), cubeSizeWithHalo(cubesize + 2)
      {
	// +2 for the halo of empty valid locations around the cube
	latDat = FourCubeLatticeData::Create(Comms(), cubesize + 2);
	simConfig = new OneInOneOutSimConfig(path);
	simState = std::make_unique<lb::SimulationState>(simConfig->GetTimeStepLength(),
							 simConfig->GetTotalTimeSteps());
	lbmParams = new lb::LbmParameters(simState->GetTimeStepLength(),
					  simConfig->GetVoxelSize());
	unitConverter = &simConfig->GetUnitConverter();

	initParams.latDat = latDat;
	initParams.siteCount = initParams.latDat->GetLocalFluidSiteCount();
	initParams.lbmParams = lbmParams;
	numSites = initParams.latDat->GetLocalFluidSiteCount();
      }

      FourCubeBasedTestFixtureBase::~FourCubeBasedTestFixtureBase() {
	delete latDat;
	delete lbmParams;
	delete simConfig;
      }

    }
  }
}
