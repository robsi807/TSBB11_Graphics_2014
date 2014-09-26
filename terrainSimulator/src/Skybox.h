//_________________________________skybox.h_________________________________//
// Filename: skybox.h					                    //
// Description: .h file for the skybox class. Includes class definition.    //
// Author: Carl Stejmar	(creator)                                           //
// Date: 2014-09-17       				                    //
//__________________________________________________________________________//

#ifndef _SKYBOX_
#define _SKYBOX_

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

class Skybox
{
 private:
  TextureData texture[6];
  Model* model;
  GLuint* shader;

 public:
  Skybox(GLuint* skyboxShader,mat4 projectionMatrix, const char *imagePath);
  void loadImages(const char *imagePath);
  void generateCubeMap();
  void draw(mat4 cameraMatrix);
};

#endif
