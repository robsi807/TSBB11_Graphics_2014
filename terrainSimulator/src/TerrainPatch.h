#ifndef _TERRAINPATCH_
#define _TERRAINPATCH_

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

#include <iostream>
#include "LoadTGA.h"
#include "loadobj.h"
#include "VectorUtils3.h"
#include "Plant.h"

#include<vector>

using namespace std;

class TerrainPatch
{
 private:
  GLuint *terrainShader,*grassShader;
  GLuint* texture;

  vec3 calcNormal(vec3 v0, vec3 v1, vec3 v2);
  
  bool geometryBoolean;
  

 public:
  int size,blendedSize,patchOverlap; 

  // Position in world coordinates
  int xPos, yPos;

  // Position in grid
  int xGrid,yGrid;

  // The height maps
  vector<float> rawHeightMap;
  vector<float> blendedHeightMap;
  
  // Objects in the patch
  vector<Plant*> objects; 
  
  // Geometry
  Model* geometry;

  // Constructor and destructor
  TerrainPatch(vector<float> initHeightMap,int patchSize, int x, int y, int overlap, GLuint* phongShader, GLuint *terrainTexture,Model* plantModel);
  ~TerrainPatch();

  // Functions
  float calcHeight(float x,float z,int texWidth);
  void generateGeometry();
  void draw(mat4 cameraMatrix,float time);
  void uploadGeometry();
  void generateAndUploadGeometry();
  bool hasGeometry();

};

#endif
