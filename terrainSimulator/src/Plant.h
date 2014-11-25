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
#include "Camera.h"

using namespace std;

class Plant{

 public:
  vec3 position;
  mat4 mdl2World;
  float scale;
  Model *model;
  GLuint *shader,*geomShader;

  Plant(GLuint *shade,GLuint *geoShade,Model* mod,vec3 pos,float yRot,float scaling);
  void draw(Camera* cam,float time);
};

#endif
