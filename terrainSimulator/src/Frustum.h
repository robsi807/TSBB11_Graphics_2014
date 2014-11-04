#ifndef _FRUSTUM_
#define _FRUSTUM_

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
#include "Camera.h"

#include <cmath>

// Plane, Halfspace & Frustum is used for frustum culling
class Plane{
 public:
  float a,b,c,d;
  void planeNormalize();
};

typedef enum Halfspace{
  NEGATIVE = -1,
  ON_PLANE = 0,
  POSITIVE = 1
}Halfspace;

class Frustum{
 public:
  Plane planes[6];
  Frustum(class Camera* cam);
  void update(class Camera* cam);
  bool containsPatch(TerrainPatch* patch);
  bool checkPatch(TerrainPatch* patch);
};

float planeDistanceToPoint(Plane plane, vec3 pos);
vec3 planeProjectionOfPoint(Plane plane, vec3 p);
Halfspace planeClassifyPoint(Plane plane, vec3 pos);
Halfspace planeClassifySphere(Plane plane, vec3 pos, float radius);
Halfspace planeClassifyInfiniteCylinder(Plane plane, vec3 pos, float radius);

#endif

