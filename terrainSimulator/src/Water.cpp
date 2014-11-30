#include "Water.h"


void generateTextureMap(vector<float> waterMap, int mapSize);

Water::Water(vector<float> initHeightMap, int mapSize)
{
  generateWaterMap(initHeightMap, mapSize);
  generateTextureMap(waterMap, mapSize);
}

Water::Water()
{

}

void Water::generateWaterMap(vector<float> initHeightMap, int mapSize) {
  
  waterMap.resize(initHeightMap.size(), 0.0);
  //mountainMap.resize(initHeightMap.size(), 0.0);
  
  float waterMax = 0.0;
  
  for(int i = 0; i < initHeightMap.size();i++) {
    if(initHeightMap.at(i) > waterMax) {
      waterMax = initHeightMap.at(i);
    }
    if(initHeightMap.at(i) < WATER_LEVEL) {
      //cout << "WE GOT WATER LOWER THAN " << WATER_LEVEL << "\n";
      //cout << "The level is:  " << initHeightMap.at(i) << "\n";
      waterMap.at(i) = 1.0;
    }
    
    if(initHeightMap.at(i) > MOUNTAIN_LEVEL) {
      highPointIndexVector.push_back(i);
    }
  }
  
  for (int i = 0; i < NUMBER_OF_WATER_SOURCES;i++) {
    waterSources.push_back(rand() % highPointIndexVector.size());
  }
  
  cout << "generated water map! Highest point was: " << waterMax << "! \n";
  cout << "we got " << highPointIndexVector.size() << " high points! \n";
  
  
}

void generateTextureMap(vector<float> waterMap, int mapSize) {
  
  GLuint texId = (GLuint) 10;
  
  glGenTextures(1, &texId);			// Generate OpenGL texture IDs
	glBindTexture(GL_TEXTURE_2D, texId);		// Bind Our Texture
  
  float* waterMapData = &waterMap[0];

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, mapSize, mapSize, 0, GL_RED, GL_FLOAT, waterMapData);
  
  
}



void Water::draw(mat4 cameraMatrix,float time) {
  
}



Water::~Water()
{

}
