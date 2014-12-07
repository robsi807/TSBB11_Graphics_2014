#ifndef _WATER_
#define _WATER_

#ifdef __APPLE__
	#include <OpenGL/gl3.h>
	#include "../common/mac/MicroGlut.h"
#elif _WIN32
	#define GLEW_STATIC
	#include <GL/glew.h>
	#include <GL/freeglut.h>
	#include <GL/gl.h>
#elif __linux
	#include <GL/gl.h>
	#include "../common/linux/MicroGlut.h"
#endif

#include "VectorUtils3.h"
#include "loadobj.h"
#include "LoadTGA.h"

//#include <cmath>
#include <iostream>
#include <vector>
#include <math.h>


#define WATER_LEVEL 0.1
#define MOUNTAIN_LEVEL 0.2
#define NUMBER_OF_WATER_SOURCES 1

#define CUBE_SIZE 1


using namespace std;

class Water
{
 private:
  //TextureData texture[6];
 
  struct WaterCube
  {
    float xPos;
    float yPos;
    float zPos;
    int height;
    int side;
  };
  
  GLuint* waterShader;
  vector<float> waterMap;
  vector<int> highPointIndexVector; 
  vector<int> waterSources;
  vector<WaterCube> riverModel;
  int mapSize;
  
  float heightScale;
  int waterHeight;
  
  int xPos, yPos;

 public:
  Model* surface;
  
  void generateWaterMap(vector<float> initHeightMap, int mapSize);
  void draw(mat4 cameraMatrix,float time);
  void generateWater(vector<float> blendedHeightMap, int mapSize, float heightScale);
  void generateWaterModel(float heightScale);
  void uploadGeometry();
  
  
  Water(vector<float> initHeightMap, int mapSize); // not used
  Water(int xPos, int yPos,  GLuint *waterShader);
  Water();
  ~Water();

};

#endif
