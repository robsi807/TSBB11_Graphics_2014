#ifndef _PLANT_
#define _PLANT_

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
#include "WorldObject.h"

using namespace std;

enum PlantType { 
  Bush,
  Tree
};

class Plant : public WorldObject {

 public:
  // Static variables
  static Model *bushModel,*bushTrunkModel,*treeModel,*treeTrunkModel;
  static GLuint *shader;
  static GLuint *geomBushShader;
  static GLuint *geomTreeShader;
  static GLuint woodTexture;
  
  // Plant type
  PlantType type;

  // Initialize static variables
  static void initPlants(GLuint *shade,GLuint *geoBushShade,GLuint *geoTreeShade);
  
  mat4 mdl2World;
  float scale;
  vec3 globalPosition;

  Plant(vec3 pos,float yRot,float scaling,vec3 terrainPos,PlantType plantType);
  void draw(mat4 cameraMatrix,float time);
  
};

#endif
