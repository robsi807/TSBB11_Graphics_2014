#ifndef _TREE_
#define _TREE_

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

class Tree : public WorldObject {

 public:
  // Static variables
  static Model *model,*trunkModel;
  static GLuint *shader;
  static GLuint *geomShader;
  static GLuint woodTexture;
  
  // Initialize static variables
  static void initTrees(GLuint *shade,GLuint *geoShade);
  
  mat4 mdl2World;
  float scale;
  vec3 globalPosition;

  Tree(vec3 pos,float yRot,float scaling,vec3 terrainPos);
  void draw(mat4 cameraMatrix,float time);
  
};

#endif
