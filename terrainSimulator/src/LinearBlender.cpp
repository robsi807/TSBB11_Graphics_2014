#include "LinearBlender.h"

// Constructor
LinearBlender::LinearBlender(int initOverlap){
  overlap = initOverlap;
  initCornerWeight();
}

// Private functions
void LinearBlender::initCornerWeight(){
  
  // Fill cornerWeight with zeros
  // There are four corners, hence i < 4
  vector<float> tempZeros (overlap*overlap,0.0);
  for(int i = 0; i<4; i++){
    cornerWeight.push_back(tempZeros);
  }
  for(int i = 0; i<overlap;i++){
    for(int j = 0; j<overlap; j++){
      float diffX = ((float)i)/((float)overlap-1);
      float diffY = ((float)j)/((float)overlap-1);

      float interpX = interpolateValues(1,0,diffX);
        
      cornerWeight.at(0).at(overlap*i + j) = interpolateValues(interpX,0,diffY);
        
      interpX = interpolateValues(0,1,diffX);
        
      cornerWeight.at(1).at(overlap*i + j) = interpolateValues(interpX,0,diffY);

      interpX = interpolateValues(1,0,diffX);
        
      cornerWeight.at(2).at(overlap*i + j) = interpolateValues(0,interpX,diffY);

      interpX = interpolateValues(0,1,diffX);
        
      cornerWeight.at(3).at(overlap*i + j) = interpolateValues(0,interpX,diffY);
    }
  }
}

// A function simple help function for interpolating between a and b
float LinearBlender::interpolateValues(float a, float b,float x){

  float ft = x*3.1415927;
  float f = (1.0-cos(ft))*0.5;
  float res = a*(1.0-f) + b*f;
  return res;
  //return a*(1-x) + b*x;
}

void LinearBlender::blendCorners(TerrainPatch* patch00,
				 TerrainPatch* patch01,
				 TerrainPatch* patch10,
				 TerrainPatch* patch11){

  int patchSize = patch00->size;

  // Row interators for each patch
  int i00,i01,i10,i11;

  // Column iterators for each patch
  int j00,j01,j10,j11;

  // Loop over the corner
  for(int i=0; i<overlap; i++){
    for(int j=0; j<overlap; j++){
      // Recalculate iterators
      i00 = i + patchSize - overlap;
      j00 = j + patchSize - overlap;
      i01 = i00;
      j01 = j;
      i10 = i;
      j10 = j + patchSize - overlap;
      i11 = i;
      j11 = j;

      float interpValue = // cornerWeight.at(0).at(overlap*i+j);
       	cornerWeight.at(0).at(overlap*i+j) * patch00->rawHeightMap.at(patchSize*i00 + j00) +
       	cornerWeight.at(1).at(overlap*i+j) * patch10->rawHeightMap.at(patchSize*i10 + j10) +
      	cornerWeight.at(2).at(overlap*i+j) * patch01->rawHeightMap.at(patchSize*i01 + j01) +
       	cornerWeight.at(3).at(overlap*i+j) * patch11->rawHeightMap.at(patchSize*i11 + j11);

      patch00->blendedHeightMap.at(patchSize*i00 + j00) = interpValue;
      patch01->blendedHeightMap.at(patchSize*i01 + j01) = interpValue;
      patch10->blendedHeightMap.at(patchSize*i10 + j10) = interpValue;
      patch11->blendedHeightMap.at(patchSize*i11 + j11) = interpValue;

    }
  }

}

void LinearBlender::blendHors(TerrainPatch* patchWest,TerrainPatch* patchEast){
  int patchSize = patchWest->size;
  int jWest; // jEast is always = j
  for(int i=overlap; i<patchSize-overlap; i++){
    for(int j=0; j<overlap; j++){
      jWest = j + patchSize - overlap;
      float diff = ((float)j)/(overlap-1);
      float weight = interpolateValues(0,1,diff);
      float interp = 
	weight*patchEast->rawHeightMap.at(patchSize*i + j) +
	(1-weight)*patchWest->rawHeightMap.at(patchSize*i + jWest);

      patchWest->blendedHeightMap.at(patchSize*i + jWest) = interp;
      patchEast->blendedHeightMap.at(patchSize*i + j) = interp;
    }
  } 
}

void LinearBlender::blendVert(TerrainPatch* patchNorth,TerrainPatch* patchSouth){
  int patchSize = patchNorth->size;
  int iNorth; // iSouth is always = i
  for(int i=0; i<overlap; i++){
    float diff = ((float)i)/(overlap-1);
    float weight = interpolateValues(0,1,diff);
    for(int j=overlap; j<patchSize-overlap; j++){
      iNorth = i + patchSize - overlap;
      
      float interp = 
	weight*patchSouth->rawHeightMap.at(patchSize*i + j) +
	(1-weight)*patchNorth->rawHeightMap.at(patchSize*iNorth + j);

      patchNorth->blendedHeightMap.at(patchSize*iNorth + j) = interp;
      patchSouth->blendedHeightMap.at(patchSize*i + j) = interp;
    }
  }
}



