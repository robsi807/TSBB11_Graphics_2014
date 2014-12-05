#ifndef _EVADER_
#define _EVADER_

#include "../common/VectorUtils3.h"
#include "../common/loadobj.h"

#include "Boid.h"

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
  
  GLuint* shader;
  GLuint texture;

  vec3 follow, avoidChaserVector;

  GLfloat prevTime;
  vec3 tempSpeed;
  
  //vec3 maxSpeed;
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
  void updateBoundingPositions(vec3 cameraPosition);

 public:
  Model* model;
  Boid leader;
  int flockIndex;
  std::vector<Boid> evaderVector; // Change to pointers?

  Evader(GLuint *phongShader, Model *evaderModel, GLuint evaderTexture, vec3 pos, int numOfBoids, int index, vec3 cameraPosition);
  void draw(mat4 cameraMatrix);
  //void animate(GLfloat time);
  void update(GLfloat time, vector<Boid> chaserVector, vec3 cameraPosition);
  
};

#endif

