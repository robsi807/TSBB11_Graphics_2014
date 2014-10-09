#include "LinearBlender.h"

// Constructor
LinearBlender::LinearBlender(int initOverlap){
  overlap = initOverlap;
  void initCornerWeight();
  void initLinearWeights();
}

// Private functions
void LinearBlender::initCornerWeight(){
  
  // Fill cornerWeight with zeros
  // There are four corners, hence i < 4
  vector<float> tempZeros (0.0,overlap*overlap);
  for(int i = 0; i<4; i++){
    cornerWeight.push_back(tempZeros);
  }

  for(int i = 0; i<overlap;i++){
    for(int j = 0; j<overlap; j++){
      float diffX = (i-1)/(overlap-1);
      float diffY = (j-1)/(overlap-1);

      float interpX = interpolateValues(1,0,diffX);
        
      cornerWeight.at(1).at(overlap*i + j) = interpolateValues(interpX,0,diffY);
        
      interpX = interpolateValues(0,1,diffX);
        
      cornerWeight.at(1).at(overlap*i + j) = interpolateValues(interpX,0,diffY);

      interpX = interpolateValues(1,0,diffX);
        
      cornerWeight.at(1).at(overlap*i + j) = interpolateValues(0,interpX,diffY);

      interpX = interpolateValues(0,1,diffX);
        
      cornerWeight.at(1).at(overlap*i + j) = interpolateValues(0,interpX,diffY);
    }
  }
}

void LinearBlender::initLinearWeights(){
  
}

// A function simple help function for interpolating between a and b
float LinearBlender::interpolateValues(float a, float b,float x){
  return a*(1-x) + b*x;
}

void LinearBlender::blendCorners(TerrainPatch* patch00,
				 TerrainPatch* patch01,
				 TerrainPatch* patch10,
				 TerrainPatch* patch11){
  
  

}

void LinearBlender::blendHors(TerrainPatch* patchWest,TerrainPatch* patchEast){

}

void LinearBlender::blendVert(TerrainPatch* patchNorth,TerrainPatch* patchSouth){

}



