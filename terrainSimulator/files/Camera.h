
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

class Camera
{
 private:
  
  vec3 camPosition = vec3(0,0,0);
  vec3 lookAtPoint = vec3(0,0,0);
  vec3 upVector = vec3(0,1,0);
  GLfloat velocity;
  GLfloat sensitivity;
  int windowWidth;
  int windowHeight;

 public:
  mat4 camMatrix;
  Camera();
  //Camera(vec3 pos);
  void init(vec3 pos, int width, int height, GLfloat vel, GLfloat sens);
  void handleKeyPress();
  void handleMouse(int x, int y);
  void update();
};

#endif

