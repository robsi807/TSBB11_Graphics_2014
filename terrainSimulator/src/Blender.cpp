#include "Blender.h"

// Constructor
Blender::Blender(int initOverlap){
  overlap = initOverlap;
  initCornerWeight();
}

// Private functions
void Blender::initCornerWeight(){
  
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
float Blender::interpolateValues(float a, float b,float x){

  float ft = x*3.1415927;
  float f = (1.0-cos(ft))*0.5;
  float res = a*(1.0-f) + b*f;
  return res;
  //return a*(1-x) + b*x;
}

void Blender::blendCorners(TerrainPatch* patch00,
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

void Blender::blendHors(TerrainPatch* patchWest,TerrainPatch* patchEast){
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

void Blender::blendVert(TerrainPatch* patchNorth,TerrainPatch* patchSouth){
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

void Blender::blendNorth(vector<vector<TerrainPatch*>> *terrainVector) {
  // Blend in the new terrain
  
  int xSize = terrainVector->at(0).size();
  int ySize = terrainVector->size(); // 
  
  
  for(int x = 0; x < xSize; x++){
    TerrainPatch *p00,*p01,*p10,*p11;
    p01 = terrainVector->at(ySize-2).at(x); // ySize is not zero indexed
    p11 = terrainVector->at(ySize-1).at(x); // ySize is not zero indexed
    blendVert(p01,p11);
      
    if(x != 0){
      p00 = terrainVector->at(ySize-2).at(x-1); // vector is zero indexed
      p10 = terrainVector->at(ySize-1).at(x-1); // vector is zero indexed
      
      blendHors(p10,p11);
      blendCorners(p00,p01,p10,p11);
    }
  }
}

void Blender::blendSouth(vector<vector<TerrainPatch*>> *terrainVector) {

  int xSize = terrainVector->at(0).size();
  int ySize = terrainVector->size(); // 

  // Blend in the new terrain
  
  for(int x = 0; x < xSize; x++){
    TerrainPatch *p00,*p01,*p10,*p11;
    p01 = terrainVector->at(0).at(x);
    p11 = terrainVector->at(1).at(x);
    blendVert(p01,p11);
      
    if(x != 0){
      p00 = terrainVector->at(0).at(x-1);
      p10 = terrainVector->at(1).at(x-1);
      
      blendHors(p10,p11);
      blendCorners(p00,p01,p10,p11);
    }
  }
}

void Blender::blendEast(vector<vector<TerrainPatch*>> *terrainVector) {

  int xSize = terrainVector->at(0).size();
  int ySize = terrainVector->size(); // 


  // Blend in the new terrain
  for(int y = 0; y < ySize; y++){
    TerrainPatch *p00,*p01,*p10,*p11;
    p10 = terrainVector->at(y).at(0);
    p11 = terrainVector->at(y).at(1);
    blendHors(p10,p11);
      
    if(y != 0){
      p00 = terrainVector->at(y-1).at(0);
      p01 = terrainVector->at(y-1).at(1);
      blendVert(p01,p11);
      blendCorners(p00,p01,p10,p11);
    }
  }



}

void Blender::blendWest(vector<vector<TerrainPatch*>> *terrainVector) { 
  int xSize = terrainVector->at(0).size();
  int ySize = terrainVector->size(); // 
  
  // Blend in the new terrain
  for(int y = 0; y < ySize; y++){
    TerrainPatch *p00,*p01,*p10,*p11;
    p10 = terrainVector->at(y).at(xSize-2);
    p11 = terrainVector->at(y).at(xSize-1);
    blendHors(p10,p11);
      
    if(y != 0){
      p00 = terrainVector->at(y-1).at(xSize-2);
      p01 = terrainVector->at(y-1).at(xSize-1);
      blendVert(p01,p11);
      blendCorners(p00,p01,p10,p11);
    }
  }
    

}


void Blender::blendAll(vector<vector<TerrainPatch*>> *terrainVector) {

    // Blend!
  cout << "Blending patches... " << endl;
  for(int y = 0; y < terrainVector->size(); y++){
    for(int x = 0; x < terrainVector->at(y).size(); x++){
      // Blend horizontal
      if(x != 0){
	      TerrainPatch *pWest,*pEast;
	      pWest = terrainVector->at(y).at(x-1);
	      pEast = terrainVector->at(y).at(x);
	      blendHors(pWest,pEast);
      }
      // Blend vertical
      if(y != 0){
	      TerrainPatch *pSouth,*pNorth;
	      pSouth = terrainVector->at(y-1).at(x);
	      pNorth = terrainVector->at(y).at(x);
	      blendVert(pSouth,pNorth);
      }
      // Blend corners
      if(y != 0 && x != 0){
	      TerrainPatch *p00,*p01,*p10,*p11;
	      p00 = terrainVector->at(y-1).at(x-1);
	      p01 = terrainVector->at(y-1).at(x);
	      p10 = terrainVector->at(y).at(x-1);
	      p11 = terrainVector->at(y).at(x);
	      blendCorners(p00,p01,p10,p11);
      }    
    }
  }
}




