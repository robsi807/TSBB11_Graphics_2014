
#include "Chaser.h"

Chaser::Chaser(GLuint *phongShader, Model *chaserModel, GLuint chaserTexture, vec3 pos, int numOfBoids, vec3 cameraPosition)
{
  shader = phongShader;

  model = chaserModel;
  texture = chaserTexture;

  awarenessRadius = 80.0;
  minDistance = 40.0;

  maxSpeed = Norm(vec3(1.6,1.6,1.6)); // Not used now, change to this in checkMaxSpeed

  avoidanceWeight = 0.04;//0.04; //0.02, 0.2
  attackWeight = 0.005; //0.004;
  nearestWeight = 0.009;

  lowInterval = 0.0;
  highInterval = 0.5;

  // Bounding the positions inside this cube. Should maybe center around camera position instead.
  xMin = cameraPosition.x - 256.0;
  xMax = cameraPosition.x + 256.0;
  yMin = 30.0; // should be taken from calcHeight
  yMax = 300.0; // Should be a fixed value because otherwise the followCam will make it possible to fly up for infinity.
  zMin = cameraPosition.z - 256.0;
  zMax = cameraPosition.z + 256.0;
  
  makeIndividuals(numOfBoids, pos);
}

/*Chaser::~Chaser()
{

}*/

void Chaser::draw(mat4 cameraMatrix)
{
  int numberOfChasers = chaserVector.size();
  for(int i = 0; i < numberOfChasers; i++)
    {
      chaserVector.at(i).draw(cameraMatrix, shader, model, &texture);
    }
}

// void Chaser::animate(GLfloat time)
// {
//   float xSpeed = cos(time);
//   float ySpeed = 2*sin(time);
//   float zSpeed = sin(time);

//   //speed = vec3(xSpeed,ySpeed,zSpeed);

//   //Create bone class to perform skinning. Insert bone to model here and animate.

//   position += speed;
// }

void Chaser::setRandomSpeed(Boid *boidI, GLfloat time)
{
  // boost::random::random_device rd;
  // //boost::mt19937 generator(rd());
  // boost::random::mt19937 generator(rd());
  // //std::uniform_real_distribution<> dis(-1, 1);
  // //std::default_random_engine generator;
  // boost::random::uniform_real_distribution<float> distribution(lowInterval, highInterval); //(-1.0, 1.0)

  // float xOffset = distribution(generator);
  // float yOffset = distribution(generator);
  // float zOffset = distribution(generator);

  //boost::random::random_device rd;
  //boost::mt19937 generator(rd());
  boost::random::mt19937 generator(time);
  //std::uniform_real_distribution<> dis(-1, 1);
  //std::default_random_engine generator;
  boost::random::uniform_real_distribution<float> distribution(lowInterval, highInterval); //(-1.0, 1.0)
  boost::variate_generator<boost::random::mt19937, boost::random::uniform_real_distribution<float>> dice(generator, distribution);

  float xOffset = dice();//distribution(generator);
  float yOffset = dice();//distribution(generator);
  float zOffset = dice();//distribution(generator);

  // cout << "xOffset = " << xOffset << " zOffset = " << zOffset << endl;
  // cout << "lowInterval = " << lowInterval << endl;
  // cout << "highInterval = " << highInterval << endl;

  vec3 offsetVec = vec3(xOffset, yOffset/2.0, zOffset); // for testing: vec3(0,0,0);
  boidI->speed += offsetVec/2.0;

  //if(Norm(boidI->speed) > maxSpeed)
  //boidI->speed /= 2.0;

  //boidI->position += boidI->speed;
}

void Chaser::checkMaxSpeed(Boid *boid)
{
  vec3 mSpeed = vec3(1.2,1.2,1.2);
if(Norm(boid->speed) > Norm(mSpeed))
    {
      boid->speed = (boid->speed/Norm(boid->speed))*Norm(mSpeed);

      // if(boid->speed.x < 0)
      // 	boid->speed.x = -1*mSpeed.x;
      // if(boid->speed.x > 0)
      // 	boid->speed.x = mSpeed.x;

      // if(boid->speed.y < 0)
      // 	boid->speed.y = -1*mSpeed.y;
      // if(boid->speed.y > 0)
      // 	boid->speed.y = mSpeed.y;

      // if(boid->speed.z < 0)
      // 	boid->speed.z = -1*mSpeed.z;
      // if(boid->speed.z > 0)
      // 	boid->speed.z = mSpeed.z;
      
    }
}

void Chaser::updateBoundingPositions(vec3 cameraPosition)
{
  // Bounding the positions inside this cube.
  xMin = cameraPosition.x - 256.0;
  xMax = cameraPosition.x + 256.0;
  //yMin = 30.0; // should be taken from calcHeight
  //yMax = 300.0; // Should be a fixed value because otherwise the followCam will make it possible to fly up for infinity.
  zMin = cameraPosition.z - 256.0;
  zMax = cameraPosition.z + 256.0;
}

void Chaser::update(GLfloat time, int chaserIndex, vector<Boid> evaderVector, vec3 cameraPosition)
{
  /*cout << "Position for chaser: ("
       << chaserVector.at(0).position.x << ","
       << chaserVector.at(0).position.y << ","
       << chaserVector.at(0).position.z << ")" << endl;*/
  updateBoundingPositions(cameraPosition);

  searchPrey(chaserIndex, evaderVector, time);
}

void Chaser::boundPositionBoid(Boid *boid)
{
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

  else if(boid->position.y < yMin)
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

void Chaser::searchPrey(int chaserIndex, vector<Boid> evaderVector, GLfloat time)
{
  int N = chaserVector.size();

  if(N > 1)
    avoidance(&chaserVector.at(chaserIndex), chaserIndex);
      
  attack(&chaserVector.at(chaserIndex),evaderVector);
  setRandomSpeed(&chaserVector.at(chaserIndex), time);

  chaserVector.at(chaserIndex).speed += chaserVector.at(chaserIndex).avoidanceVector*avoidanceWeight + attackVector*attackWeight + nearest*nearestWeight;
      
  chaserVector.at(chaserIndex).direction = chaserVector.at(chaserIndex).speed; // Normalize?
  chaserVector.at(chaserIndex).setRotation();

  // if(Norm(chaserVector.at(chaserIndex).speed) > maxSpeed)
  //   chaserVector.at(chaserIndex).speed /= 2.0;
  checkMaxSpeed(&chaserVector.at(chaserIndex));

  chaserVector.at(chaserIndex).position += chaserVector.at(chaserIndex).speed;
  boundPositionBoid(&chaserVector.at(chaserIndex));
   
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
	      //float fx = exp(1.0/distance) - 1.0;
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
  float viewAngle = 3.0; // 1.05 = 60 degrees //0.52 radians = 0.52*(180/pi) = 30 degrees //([-20,20])
  vec3 directionToEvader = evader.position - chaser.position;
  if(Norm(directionToEvader) < awarenessRadius)
    {
      float angle = atan2(chaser.direction.x - directionToEvader.x, chaser.direction.z - directionToEvader.z);
  
      if(abs(angle) < viewAngle)
	{
	  return true;
	  cout << "In Chaser class: Evader inside view!!!" << endl;
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
	  //cout << "Inside view!" << endl;
	  if(Norm(evaders.at(i).position - boidI->position) < minD)
	    {
	      minD = Norm(evaders.at(i).position - boidI->position);
	      minIndex = i;
	    }
	  attackVector += evaders.at(i).position - boidI->position;
	  count++;
	}
    }
  //cout << "In chaser class: COUNT = " << count << endl;
  if(count > 0)
    attackVector /= (float)count;
  //boidI->direction = Normalize(boidI->direction);

  if(minIndex != -1)
    nearest = evaders.at(minIndex).position - boidI->position;
}

// void Chaser::attackNearest(Boid* boidI, vector<Boid> evaders)
// {
//   nearest = vec3(0,0,0);
//   int N = evaders.size();

//   float minD = awarenessRadius;
//   int minIndex = -1;
//   for(int i = 0; i < N; i++)
//     {
//       if(Norm(evaders.at(i).position - boidI->position) < awarenessRadius)
// 	{
// 	  if(Norm(evaders.at(i).position - boidI.position) < minD)
// 	    {
// 	      minD = Norm(evaders.at(i).position - boidI.position);
// 	      minIndex = i;
// 	    }
// 	}
//     }

//   nearest = evaders.at(minIndex).position - boidI->position;
// }

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
