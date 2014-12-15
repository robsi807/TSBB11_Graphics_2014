//____________________________Boid.h____________________________
// Description: Header file for boids
// Author: Carl Stejmar, carst056@student.liu.se
// Date: 2014-12-10
//______________________________________________________________

#ifndef _BOID_
#define _BOID_

#include "../common/VectorUtils3.h" // OpenGL included here!
#include "../common/loadobj.h"

#include <cmath>
#include <iostream>

class Boid
{
 private:

 public:

  vec3 position, averagePosition;
  vec3 speed, speedDifference;
  vec3 avoidanceVector, cohesionVector, alignmentVector;
  vec3 forward, up;
  vec3 direction;

  uint animationIndex;

  mat4 rotationMatrix;

  Boid();
  Boid(vec3 pos);
   ~Boid();
  void setRotation();
  void draw(mat4 cameraMatrix, GLuint* shader, Model* model, GLuint* texture);
};

#endif

