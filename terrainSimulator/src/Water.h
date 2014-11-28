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


#define WATER_LEVEL 0.1


using namespace std;

class Water
{
 private:
  //TextureData texture[6];
 
  GLuint* shader;
  vector<float> waterMap; 
  int mapSize;

 public:
  Model* model;
  
  void generateWaterMap(vector<float> initHeightMap, int mapSize);
  Water(vector<float> initHeightMap, int mapSize);
  void draw(mat4 cameraMatrix,float time);
  Water();
  ~Water();

};

#endif
