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
#include "../common/GL_utilities.h"

#include <cmath>
#include <iostream>

class Camera
{
 private:
  vec3 position;
  vec3 lookAtPoint;
  vec3 upVector;

  GLfloat velocity;
  GLfloat sensitivity;

  // Perspective
  float projectionLeft;
  float projectionRight;
  float projectionBottom;
  float projectionTop;
  float projectionNear;
  float projectionFar;

 public:

  static const int SCREEN_WIDTH = 1024;
  static const int SCREEN_HEIGHT = 860;

  mat4 cameraMatrix;
  mat4 projectionMatrix;
  Camera(vec3 pos, GLfloat vel, GLfloat sens);
  Camera(float left, float right, float bottom, float top, float near, float far);
  void handleKeyPress();
  void handleMouse(int x, int y);
  void update();
};

#endif

