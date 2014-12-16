//____________________________Evader.cpp____________________________
// Description: Class functions for evaders
// Author: Carl Stejmar, carst056@student.liu.se
// Date: 2014-12-10
//__________________________________________________________________

#include "Evader.h"

Evader::Evader(vec3 pos, int numOfBoids, int index, vec3 cameraPosition)
{
  maxDistance = 20.0;
  minDistance = 5.0;
  awarenessRadius = 40;

  cohesionWeight = 0.002;
  avoidanceWeight = 0.1;
  alignmentWeight = 0.001;
  followWeight = 0.002;
  avoidChaserWeight = 0.4;
  avoidChaserWeightLeader = 0.008;

  // Bounding the positions inside this cube.
  xMin = cameraPosition.x - 256.0;
  xMax = cameraPosition.x + 256.0;
  // yMin is calculated by calcHeight.
  // yMax should be a fixed value because otherwise the followCam
  // will make it possible to fly up for infinity.
  yMax = 250.0;
  zMin = cameraPosition.z - 256.0;
  zMax = cameraPosition.z + 256.0;

  cohesionCount = 0;
  avoidanceCount = 0;
  alignmentCount = 0;
  
  makeFlockOf(numOfBoids, pos);
  leader.position = pos + vec3(50,0,50);

  prevTime = 0.0;
  flockIndex = index;
}

Evader::~Evader()
{
  // The vector capacity is not guaranteed to change due
  // to calling this function.
  evaderVector.clear();
  //evaderVector.erase(chaserVector.begin(), chaserVector.end()); 
}

// void Evader::draw(mat4 cameraMatrix)
// {
//   int numberOfEvaders = evaderVector.size();
//   for(int i = 0; i < numberOfEvaders; i++)
//     {
//       evaderVector.at(i).draw(cameraMatrix, shader, model, &texture);
//     }
//   //leader.draw(cameraMatrix, shader, model, &texture);
// }

void Evader::updateLeader(GLfloat time)
{
  // To make the flock not change direction so often, update offsetVec every 1.5 sec.
  if((time - prevTime) > 1.5)
    {
      prevTime = time;

      float lowInterval = -1.0;
      float highInterval = 1.0;

      boost::random::mt19937 generator(rand());
      boost::random::uniform_real_distribution<float> distribution(lowInterval, highInterval);

      float xOffset = distribution(generator);
      float yOffset = distribution(generator);
      float zOffset = distribution(generator);

      vec3 offsetVec = vec3(xOffset/2.0, yOffset/4.0, zOffset/2.0); // for testing: vec3(0,0,0);
      tempSpeed = offsetVec;
    }

  leader.speed += tempSpeed;

  checkMaxSpeed(&leader);
  // if(Norm(leader.speed) > maxSpeed)
  //   leader.speed /= 2.0;

  leader.position += leader.speed;

  boundPositionBoid(&leader);
}

void Evader::checkMaxSpeed(Boid *boid)
{
  vec3 mSpeed = vec3(0.7,0.7,0.7);
  if(Norm(boid->speed) > Norm(mSpeed))
    {
      boid->speed = (boid->speed/Norm(boid->speed))*Norm(mSpeed);
    }
}

void Evader::updateBoundingPositions(Camera cam)
{
  // Bounding the positions inside this cube.
  xMin = cam.position.x - 256.0;
  xMax = cam.position.x + 256.0;
  yMin = cam.getActualHeight(position) + 25.0;
  // yMax is fixed. Set in constructor
  zMin = cam.position.z - 256.0;
  zMax = cam.position.z + 256.0;
}

void Evader::update(GLfloat time, vector<Boid> chaserVector, Camera cam)
{
  updateBoundingPositions(cam);
  updateLeader(time);
  flocking(chaserVector);
}

void Evader::boundPositionBoid(Boid *boid)
{
  if(boid->position.x < xMin)
    boid->speed.x += 0.2;
  else if(boid->position.x > xMax)
    boid->speed.x -= 0.2;
  else if(boid->position.y < yMin)
    boid->speed.y += 0.2;
  else if(boid->position.y > yMax)
    boid->speed.y -= 0.2;
  else if(boid->position.z < zMin)
    boid->speed.z += 0.2;
  else if(boid->position.z > zMax)
    boid->speed.z -= 0.2;
}

void Evader::flocking(vector<Boid> chaserVector)
{
  int numberOfEvaders = evaderVector.size();
  position = vec3(0.0,0.0,0.0);
  speed = vec3(0.0,0.0,0.0);

  for(int i = 0; i < numberOfEvaders; i++)
    {
      // Reset vectors
      evaderVector.at(i).cohesionVector = vec3(0,0,0);
      evaderVector.at(i).averagePosition = vec3(0,0,0);
      evaderVector.at(i).avoidanceVector = vec3(0,0,0);
      evaderVector.at(i).alignmentVector = vec3(0,0,0);
      evaderVector.at(i).speedDifference = vec3(0,0,0);

      // Reset counts
      cohesionCount = 0;
      avoidanceCount = 0;
      alignmentCount = 0;

      for(int j = 0; j < numberOfEvaders; j++)
	{
	  if(i != j)
	    {
	      cohesion(&evaderVector.at(i), evaderVector.at(j));
	      avoidance(&evaderVector.at(i), evaderVector.at(j));
	      alignment(&evaderVector.at(i), evaderVector.at(j));
	    }
	}

      // Set cohesionVector
      if(cohesionCount > 0)
	{
	  evaderVector.at(i).averagePosition /= (float)cohesionCount;
	  float dist = Norm(evaderVector.at(i).averagePosition - evaderVector.at(i).position);
	  evaderVector.at(i).cohesionVector = Normalize(evaderVector.at(i).averagePosition - evaderVector.at(i).position)*dist;
	}
      else
	evaderVector.at(i).averagePosition = evaderVector.at(i).position;

      // Set avoidanceVector
      if(avoidanceCount > 0)
	{
	  evaderVector.at(i).avoidanceVector /= (float)avoidanceCount;
	  //evaderVector.at(i).avoidanceVector = Normalize(evaderVector.at(i).avoidanceVector);
	}
      // Set alignmentVector
      if(alignmentCount > 0)
	{
	   evaderVector.at(i).alignmentVector =  evaderVector.at(i).speedDifference/(float)alignmentCount;
	  // evaderVector.at(i).alignmentVector = Normalize( evaderVector.at(i).alignmentVector);
	}
 
      avoidChaser(&evaderVector.at(i), chaserVector);
      followLeader(&evaderVector.at(i));

      evaderVector.at(i).speed += evaderVector.at(i).cohesionVector*cohesionWeight +
	evaderVector.at(i).avoidanceVector*avoidanceWeight +
      	evaderVector.at(i).alignmentVector*alignmentWeight +
	follow*followWeight + avoidChaserVector*avoidChaserWeight;

      // Should make the boids rotate in right direction when attacked
      evaderVector.at(i).direction = evaderVector.at(i).speed;

      checkMaxSpeed(&evaderVector.at(i));
      // if(Norm(evaderVector.at(i).speed) > maxSpeed)
      // 	evaderVector.at(i).speed /= 2.0;

      evaderVector.at(i).setRotation();

      evaderVector.at(i).position += evaderVector.at(i).speed;
      boundPositionBoid(&evaderVector.at(i));

      // Flock position
      position +=  evaderVector.at(i).position;
      speed +=  evaderVector.at(i).speed;
   
      //cout << "Position for boid i: (" << evaderVector.at(i).position.x << "," << evaderVector.at(i).position.y << "," << evaderVector.at(i).position.z << ")" << endl;
    }

  if(numberOfEvaders != 0)
    {
      position /= (float)numberOfEvaders;
      speed /= (float)numberOfEvaders;
    }
}

// Inside chasers view angle, xz-plane
bool Evader::insideView(Boid boidI, Boid boidJ, float radius)
{
  float viewAngle = 3.0; // ([-3,3]) radians
  vec3 directionToEvader = boidJ.position - boidI.position;
  if(Norm(directionToEvader) < radius) // radius: ex. maxDistance, awarenessRadius, minDistance
    {
      float angle = atan2(boidI.direction.x - directionToEvader.x, boidI.direction.z - directionToEvader.z);
  
      if(abs(angle) < viewAngle)
	{
	  return true;
	  //cout << "In Evader class: Evader inside view!!!" << endl;
	}
    }
  return false;
}

// Should move the for-loop to flocking() and have boidJ, cohesionCount as arguments and as well the if(i != j) outside.
void Evader::cohesion(Boid* boidI, Boid boidJ)
{  
  float distance = Norm(boidJ.position - boidI->position);
  if(distance < maxDistance)
    {
      boidI->averagePosition += boidJ.position;
      cohesionCount++;
    }
}

// Possible to optimize this by checking if flock position is near chaser first.
void Evader::avoidChaser(Boid* boidI, vector<Boid> chaserVector)
{
  avoidChaserVector = vec3(0,0,0);
  int N = chaserVector.size();
  uint count = 0;

  for(int j = 0; j < N; j++)
    {
      Boid chaserJ = chaserVector.at(j);
      
      vec3 distanceVector = chaserJ.position - boidI->position;
      float distance = Norm(distanceVector);
      if(distance < awarenessRadius)
	{
	  if(distance != 0.0)
	    avoidChaserVector -= distanceVector/distance;
	  else
	    avoidChaserVector -= distanceVector;
	  count++;
	}
    }
  if(count > 0)
    {
      avoidChaserVector /= (float)count;
      leader.speed += avoidChaserVector*avoidChaserWeightLeader;
    }
}

void Evader::avoidance(Boid* boidI, Boid boidJ)
{
  vec3 distanceVector = boidJ.position - boidI->position;
  float distance = Norm(distanceVector);
  if(distance < minDistance)
    {
      boidI->avoidanceVector -= distanceVector/distance;
      avoidanceCount++;
    }
}

void Evader::alignment(Boid* boidI, Boid boidJ)
{
  float distance = Norm(boidJ.position - boidI->position);
  if(distance < maxDistance)
    {
      boidI->speedDifference += (boidJ.speed - boidI->speed);
      alignmentCount++;
    }
}

void Evader::followLeader(Boid* boidI)
{
  boidI->direction = leader.position - boidI->position;

  if(Norm(boidI->averagePosition - boidI->position) < maxDistance)
    follow = boidI->direction;
  else
    follow = vec3(0,0,0);
}

void Evader::makeFlockOf(int inhabitants, vec3 position)
{
  for(int i = 0; i < inhabitants; i++)
    {
      vec3 pos = position;
      pos.x += (float)(rand() % 100)/5.0;
      pos.y += (float)(rand() % 50)/5.0;
      pos.z += (float)(rand() % 100)/5.0;
      //Boid *bird = new Boid(pos);
      evaderVector.push_back(Boid(pos));
    }
}
