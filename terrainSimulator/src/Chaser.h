//____________________________Chaser.h____________________________
// Description: Header file for chasers
// Author: Carl Stejmar, carst056@student.liu.se
// Date: 2014-12-10
//__________________________________________________________________

#ifndef _CHASER_
#define _CHASER_

#include "../common/VectorUtils3.h"
#include "../common/loadobj.h"

#include "Boid.h"
#include "Camera.h"

#include <boost/random/uniform_real_distribution.hpp>
#include <boost/random/mersenne_twister.hpp>

#include <cmath>
#include <iostream>
#include <vector>
#include <random>

using namespace std;

class Chaser
{
 private:
  void makeIndividuals(int inhabitants, vec3 position);
  void searchPrey(int chaserIndex, vector<Boid> evaderVector, GLfloat time, Camera cam);
  void avoidance(Boid* boidI, int index);
  void attack(Boid* boidI, vector<Boid> evaders);
  void boundPositionBoid(Boid *boid, Camera cam);
  void checkMaxSpeed(Boid *boid);
  void setRandomSpeed(Boid *boidI, GLfloat time);
  bool insideView(Boid chaser, Boid evader);
  void updateBoundingPositions(Camera cam);

  vec3 attackVector, nearest;

  GLfloat prevTime;
  vec3 tempSpeed;
  
  float awarenessRadius, minDistance;
  float avoidanceWeight, attackWeight, nearestWeight;

  float lowInterval, highInterval;

  // Bounding box
  float xMin, xMax;
  float yMin, yMax;
  float zMin, zMax;

 public:
  std::vector<Boid> chaserVector; // Change to pointers?

  Chaser(vec3 pos, int numOfBoids, vec3 cameraPosition);
  ~Chaser();
  //void draw(mat4 cameraMatrix);
  void update(GLfloat time, int chaserIndex, vector<Boid> evaderVector, Camera cam);
};

#endif

