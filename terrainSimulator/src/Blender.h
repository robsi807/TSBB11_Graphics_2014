#ifndef _BLENDER_
#define _BLENDER_

#include "TerrainPatch.h"

class Blender{

 public:
  int overlap;
  virtual void blendCorners(TerrainPatch* patch00,
		    TerrainPatch* patch01,
		    TerrainPatch* patch10,
		    TerrainPatch* patch11) = 0;

  virtual void blendHors(TerrainPatch* patchWest,TerrainPatch* patchEast) = 0;
  virtual void blendVert(TerrainPatch* patchNorth,TerrainPatch* patchSouth) = 0;
};

#endif
