#ifndef _LINEAR_BLENDER_
#define _LINEAR_BLENDER_

#include "LoadTGA.h"
#include "Blender.h"
#include "TerrainPatch.h"
#include <vector>
#include <iostream>

class LinearBlender : public Blender{
 private:
  vector<vector<float>> cornerWeight;

  void initCornerWeight();
  float interpolateValues(float a,float b,float x);
  
 public:
  LinearBlender(int initOverlap);

  void blendCorners(TerrainPatch* patch00,
		    TerrainPatch* patch01,
		    TerrainPatch* patch10,
		    TerrainPatch* patch11);

  void blendHors(TerrainPatch* patchWest,TerrainPatch* patchEast);
  void blendVert(TerrainPatch* patchNorth,TerrainPatch* patchSouth);
  
  void blendNorth(vector<vector<TerrainPatch*>> *terrainVector);
  void blendSouth(vector<vector<TerrainPatch*>> *terrainVector);
  void blendEast(vector<vector<TerrainPatch*>> *terrainVector);
  void blendWest(vector<vector<TerrainPatch*>> *terrainVector);
  
  void blendAll(vector<vector<TerrainPatch*>> *terrainVector);
  
};



#endif
