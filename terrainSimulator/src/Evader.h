//____________________________Evader.h____________________________
// Description: Header file for evaders
// Author: Carl Stejmar, carst056@student.liu.se
// Date: 2014-12-10
//__________________________________________________________________

#ifndef _EVADER_
#define _EVADER_

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

class Evader
{
 private:
  vec3 follow, avoidChaserVector;

  GLfloat prevTime;
  vec3 tempSpeed;
  
  float maxDistance, minDistance, maxSpeed, awarenessRadius;
  float cohesionWeight, avoidanceWeight, alignmentWeight, followWeight, avoidChaserWeight, avoidChaserWeightLeader;

  float xMin, xMax;
  float yMin, yMax;
  float zMin, zMax;

  void flocking(vector<Boid> chaserVector);
  void cohesion(Boid *boidI, int index);
  void avoidance(Boid* boidI, int index);
  void alignment(Boid* boidI, int index);
  void avoidChaser(Boid* boidI, vector<Boid> chaserVector);
  void followLeader(Boid* boidI);
  void boundPositionBoid(Boid *boid);
  void checkMaxSpeed(Boid *boid);
  void updateLeader(GLfloat time);
  void makeFlockOf(int inhabitants, vec3 position);
  bool insideView(Boid boidI, Boid boidJ, float radius);
  void updateBoundingPositions(Camera cam);

 public:
  vec3 position, speed;
  Boid leader;
  int flockIndex;
  std::vector<Boid> evaderVector; // Change to pointers?

  Evader(vec3 pos, int numOfBoids, int index, vec3 cameraPosition);
  ~Evader();
  //void draw(mat4 cameraMatrix);
  void update(GLfloat time, vector<Boid> chaserVector, Camera cam); 
};

#endif
