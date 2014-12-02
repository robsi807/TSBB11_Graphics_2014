
#ifndef _SUN_
#define _SUN_

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
#include "Camera.h"

//#include <cmath>
#include <stdio.h>

using namespace std;

class Sun
{
 private:
  GLuint* shader;
  GLuint textureadder,lowpass,truncate;
  vec3 position;
  FBOstruct *fbo2, *fbo3;

 public:
  GLfloat specularExponent;
  vec3 lightDir, lightDirNight;
  float distance;
  vec3 color;
  Model* model;
   Model *squareModel;
  Sun(GLuint* sunShader,mat4 projectionMatrix);
  void bloom(FBOstruct *fbo);
  void draw(mat4 cameraMatrix, GLfloat time);
  void update(GLfloat time);
};

#endif
