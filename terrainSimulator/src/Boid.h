#ifndef _BOID_
#define _BOID_

#include "../common/VectorUtils3.h" // OpenGL included here!
#include "../common/loadobj.h"

#include <cmath>
#include <iostream>

class Boid
{
 private:
  
  //~Boid();

 public:

  vec3 position, averagePosition;
  vec3 speed, speedDifference;
  vec3 avoidanceVector;
  vec3 cohesionVector;
  vec3 alignmentVector;
  vec3 forward;
  vec3 up;
  vec3 direction;

  uint animationIndex;

  mat4 rotationMatrix;

  Boid();
  Boid(vec3 pos);
  void draw(mat4 cameraMatrix, GLuint* shader, Model* model, GLuint* texture);
  void animate(GLfloat time);
  //void setDirection();
  void setRotation();
  
};

#endif

