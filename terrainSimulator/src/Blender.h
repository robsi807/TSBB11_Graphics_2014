#ifndef _BLENDER_
#define _BLENDER_

#include "LoadTGA.h"
#include "TerrainPatch.h"
#include <vector>
#include <iostream>

class Blender{
 private:
  vector<vector<float>> cornerWeight;

  void initCornerWeight();
  float interpolateValues(float a,float b,float x);
  
 public:
 
  int overlap;
 
  Blender(int initOverlap);

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
