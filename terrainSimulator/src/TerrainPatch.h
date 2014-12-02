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
#include "WorldObject.h"
#include "Camera.h"

#include "PatchGenerator.h"
#include "PerlinPatchGenerator.h"
#include "ValuePatchGenerator.h"

#include<vector>

using namespace std;

class TerrainPatch
{
 private:
  float heightScale;
  GLuint *terrainShader,*grassShader;
  GLuint* texture;

  vec3 calcNormal(vec3 v0, vec3 v1, vec3 v2);
  
  bool geometryBoolean;
  
  void addPlants();
  bool checkPlantPosition(vec3 pos);
  
 public:
  int size,blendedSize,patchOverlap; 

  // Position based random seed
  int seed;
  RNGType rng;

  // Position in world coordinates
  int xPos, yPos;

  // Position in grid
  int xGrid,yGrid;

  // The height maps
  vector<float> rawHeightMap;
  vector<float> blendedHeightMap;
  
  // Objects in the patch
  vector<class WorldObject*> objects; 
  
  // Geometry
  Model* geometry;

  PatchGenerator* patchGenerator;

  // Constructor and destructor

  TerrainPatch(int patchSize, int x, int y, int overlap, GLuint* phongShader, GLuint *terrainTexture);
  ~TerrainPatch();

  // Functions
  vec3 calcNormalSimple(float xPos, float zPos);
  float calcHeightSimple(int x,int z);
  float calcHeight(float x,float z);
  void generateGeometry();
  void draw(class Camera* cam,float time);
  void uploadGeometry();
  void generateAndUploadGeometry();
  bool hasGeometry();

};

#endif
