
#ifndef _CAMERA_
#define _CAMERA_

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

#include "../common/VectorUtils3.h"
#include "../common/loadobj.h"
#include "../common/LoadTGA.h"

//#include <cmath>
#include <stdio.h> // Inkluderas i LoadTGA.h men kan vara bra att ha ändå (?)
//#include <iostream>
//#include <vector>

using namespace std;

class Camera
{
  private:
  mat4 camMatrix;
  vec3 camPosition;
  vec3 lookAtPoint;
  vec3 upVector;


  initKeymapManager();
  vec3 r = SetVector(-0.5,0,-0.5);
  camPosition = SetVector(24,5,24);
  l = VectorAdd(p,r);
  v = SetVector(0,1,0);

  GLfloat constSpeed = 0.5;
  public:
  Skybox();
  Skybox(Model *model, GLuint shader);
  void loadImages(const char *imagePath);
  void generateCubeMap();
  void init(mat4 projectionMatrix, const char *imagePath);
  void draw(mat4 camMatrix);
};

#endif

