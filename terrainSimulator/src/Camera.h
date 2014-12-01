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

#include "VectorUtils3.h"
#include "GL_utilities.h"
#include "TerrainPatch.h"
#include "Frustum.h"
#include "Plant.h"

#include <cmath>
#include <iostream>
#include <vector>

class Camera
{
 private:
  vec3 position;
  vec3 lookAtPoint;
  vec3 upVector;
  int patchSize;
  int patchOverlap;
  int blendedSize;
  int gridSize;
  bool flying;
  std::vector<std::vector<TerrainPatch*>> *terrainVector;

  // Variables for calculating height
  int actualPatchXIndex; 
  int actualPatchZIndex;
  float groundOffset;
  TerrainPatch * actualPatch;
  std::vector<TerrainPatch*> actualPatchRow;

  GLfloat velocity;
  GLfloat sensitivity;

  bool warpPointer,lockFrustum;

  // Perspective
  float projectionLeft;
  float projectionRight;
  float projectionBottom;
  float projectionTop;
  float projectionNear;
  float projectionFar;

  class Frustum* frustumPlanes;

 public:

  static const int SCREEN_WIDTH = 1280;
  static const int SCREEN_HEIGHT = 720;
  
  mat4 cameraMatrix;
  mat4 projectionMatrix;
  Camera(vec3 pos, GLfloat vel, GLfloat sens, std::vector<std::vector<TerrainPatch*>> *terrain, int sizePatch, int overlap, int sizeGrid);
  Camera(float left, float right, float bottom, float top, float near, float far);
  void handleKeyPress();
  void handleMouse(int x, int y);
  void update();
  vec3 getDirection();
  vec3 getPosition();

  // Frustum checks
  bool isInFrustum(TerrainPatch* patch);
  bool isInFrustum(Plant* plant);

  // DEBUGGING PURPOSE CODE START
  int addTerrain;
  int terrainTimer; // Causes wierd bugs without this
  // DEBUGGING PURPOSE CODE END
};

#endif

