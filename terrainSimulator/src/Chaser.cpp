//____________________________Chaser.cpp____________________________
// Description: Class functions for chasers
// Author: Carl Stejmar, carst056@student.liu.se
// Date: 2014-12-10
//__________________________________________________________________

#include "Chaser.h"

Chaser::Chaser(vec3 pos, int numOfBoids, vec3 cameraPosition)
{
  awarenessRadius = 80.0;
  minDistance = 40.0;

  avoidanceWeight = 0.04;
  attackWeight = 0.005;
  nearestWeight = 0.009;

  lowInterval = 0.0;
  highInterval = 0.5;

  // Bounding the positions inside this cube.
  xMin = cameraPosition.x - 256.0;
  xMax = cameraPosition.x + 256.0;
  // yMin is calculated by calcHeight.
  // yMax should be a fixed value because otherwise the followCam
  // will make it possible to fly up for infinity.
  yMax = 250.0;
  zMin = cameraPosition.z - 256.0;
  zMax = cameraPosition.z + 256.0;
  
  makeIndividuals(numOfBoids, pos);

  prevTime = 0.0;
}

Chaser::~Chaser()
{
  // The vector capacity is not guaranteed to change due
  // to calling this function.
  chaserVector.clear();
  //chaserVector.erase(chaserVector.begin(), chaserVector.end()); 
}

// void Chaser::draw(mat4 cameraMatrix)
// {
//   int numberOfChasers = chaserVector.size();
//   for(int i = 0; i < numberOfChasers; i++)
//     {
//       chaserVector.at(i).draw(cameraMatrix, shader, model, &texture);
//     }
// }

void Chaser::setRandomSpeed(Boid *boidI, GLfloat time)
{
  // To make the chasers not change direction so often, update offsetVec every 1.0 sec.
  if((time - prevTime) > 1.0)
    {
      prevTime = time;
      boost::random::mt19937 generator(rand());
      boost::random::uniform_real_distribution<float> distribution(lowInterval, highInterval);

      float xOffset = distribution(generator);
      float yOffset = distribution(generator);
      float zOffset = distribution(generator);

      vec3 offsetVec = vec3(xOffset, yOffset/2.0, zOffset);
      tempSpeed = offsetVec/2.0;
    }
  boidI->speed += tempSpeed;
}

void Chaser::checkMaxSpeed(Boid *boid)
{
  vec3 mSpeed = vec3(0.9,0.9,0.9);
  if(Norm(boid->speed) > Norm(mSpeed))
    {
      boid->speed = (boid->speed/Norm(boid->speed))*Norm(mSpeed);
    }
}

void Chaser::updateBoundingPositions(Camera cam)
{
  // Bounding the positions inside this cube, yMin is
  // updated in boundPositionBoid.
  xMin = cam.position.x - 256.0;
  xMax = cam.position.x + 256.0;
  zMin = cam.position.z - 256.0;
  zMax = cam.position.z + 256.0;
}

void Chaser::update(GLfloat time, int chaserIndex, vector<Boid> evaderVector, Camera cam)
{
  updateBoundingPositions(cam);
  searchPrey(chaserIndex, evaderVector, time, cam);
}

void Chaser::boundPositionBoid(Boid *boid, Camera cam)
{
  float yMinBoid = cam.getActualHeight(boid->position) + 30.0;
  if(boid->position.x < xMin)
    {
      boid->speed.x += 0.2;
      lowInterval = 0.0;
      highInterval = 0.5;
    }
  else if(boid->position.x > xMax)
    {
      boid->speed.x -= 0.2;
      lowInterval = -0.5;
      highInterval = 0.0;
    }

  else if(boid->position.y < yMinBoid)
    boid->speed.y += 0.2;
  else if(boid->position.y > yMax)
    boid->speed.y -= 0.2;

  else if(boid->position.z < zMin)
    {
      boid->speed.z += 0.2;
      lowInterval = 0.0;
      highInterval = 0.5;
    }
  else if(boid->position.z > zMax)
    {
      boid->speed.z -= 0.2;
      lowInterval = -0.5;
      highInterval = 0.0;
    }
}

void Chaser::searchPrey(int chaserIndex, vector<Boid> evaderVector, GLfloat time, Camera cam)
{
  int N = chaserVector.size();

  if(N > 1)
    avoidance(&chaserVector.at(chaserIndex), chaserIndex);
      
  attack(&chaserVector.at(chaserIndex),evaderVector);
  setRandomSpeed(&chaserVector.at(chaserIndex), time);

  chaserVector.at(chaserIndex).speed += chaserVector.at(chaserIndex).avoidanceVector*avoidanceWeight + attackVector*attackWeight + nearest*nearestWeight;
      
  chaserVector.at(chaserIndex).direction = chaserVector.at(chaserIndex).speed; // Normalize?
  chaserVector.at(chaserIndex).setRotation();

  checkMaxSpeed(&chaserVector.at(chaserIndex));

  chaserVector.at(chaserIndex).position += chaserVector.at(chaserIndex).speed;
  boundPositionBoid(&chaserVector.at(chaserIndex),cam);
   
  //cout << "Speed for chaser at chaserIndex: (" << chaserVector.at(chaserIndex).speed.x << "," << chaserVector.at(chaserIndex).speed.y << "," << chaserVector.at(chaserIndex).speed.z << ")" << endl;
}

// Should move the for-loop to flocking() and have boidJ, cohesionCount as arguments and as well the if(i != j) outside.
void Chaser::avoidance(Boid* boidI, int index)
{
  boidI->avoidanceVector = vec3(0,0,0);
  int N = chaserVector.size();
  uint count = 0;

  for(int j = 0; j < N; j++)
    {
      Boid boidJ = chaserVector.at(j);
      if(index != j)
	{
	  vec3 distanceVector = boidJ.position - boidI->position;
	  float distance = Norm(distanceVector);
	  if(distance < minDistance)
	    {
	      if(distance != 0.0)
		boidI->avoidanceVector -= CrossProduct(distanceVector/distance,boidI->up); //distanceVector/distance;
	      else
		boidI->avoidanceVector -= CrossProduct(distanceVector,boidI->up); //distanceVector;
	      count++;
	    }
	}
    }
  if(count > 0)
    {
      boidI->avoidanceVector /= (float)count;
      //boidI->avoidanceVector = Normalize(boidI->avoidanceVector);
    }
}

// Inside chasers view angle, xz-plane
bool Chaser::insideView(Boid chaser, Boid evader)
{
  float viewAngle = 3.0; // 3.0 = 172 degrees (circa)
  vec3 directionToEvader = evader.position - chaser.position;
  if(Norm(directionToEvader) < awarenessRadius)
    {
      float angle = atan2(chaser.direction.x - directionToEvader.x, chaser.direction.z - directionToEvader.z);
  
      if(abs(angle) < viewAngle)
	{
	  return true;
	}
    }
  return false;
}

void Chaser::attack(Boid* boidI, vector<Boid> evaders)
{
  attackVector = vec3(0,0,0);
  nearest = vec3(0,0,0);
  int N = evaders.size();
  uint count = 0;

  float minD = awarenessRadius;
  int minIndex = -1;

  for(int i = 0; i < N; i++)
    {
      if(insideView(*boidI,evaders.at(i)))
	{
	  if(Norm(evaders.at(i).position - boidI->position) < minD)
	    {
	      minD = Norm(evaders.at(i).position - boidI->position);
	      minIndex = i;
	    }
	  attackVector += evaders.at(i).position - boidI->position;
	  count++;
	}
    }

  if(count > 0)
    attackVector /= (float)count;

  if(minIndex != -1)
    nearest = evaders.at(minIndex).position - boidI->position;
}

void Chaser::makeIndividuals(int inhabitants, vec3 position)
{
  for(int i = 0; i < inhabitants; i++)
    {
      vec3 pos = position;
      pos.x += (float)(rand() % 100)/5.0;
      pos.y += (float)(rand() % 50)/5.0;
      pos.z += (float)(rand() % 100)/5.0;
      //Boid *bird = new Boid(pos);
      chaserVector.push_back(Boid(pos));
    }
}
