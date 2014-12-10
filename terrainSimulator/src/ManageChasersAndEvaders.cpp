//____________________________ManageChasersAndEvaders.cpp____________________________
// Description: Class functions to handle chasers (predators) and evaders (flocks).
// Author: Carl Stejmar, carst056@student.liu.se
// Date: 2014-12-10
//___________________________________________________________________________________

#include "ManageChasersAndEvaders.h"

ManageChasersAndEvaders::ManageChasersAndEvaders(GLuint* phongShader, char *modelPathEvader, char *imagePathEvader, char *modelPathChaser, char *imagePathChaser, Camera cam)
{
  shader = phongShader;

  glUseProgram(*shader);

  //________________Evader________________
  glActiveTexture(GL_TEXTURE8);
  glGenTextures(1, &evaderTexture);
  glBindTexture(GL_TEXTURE_2D, evaderTexture);
  LoadTGATextureSimple(imagePathEvader, &evaderTexture);
  
  glUniform1i(glGetUniformLocation(*shader, "evaderTexture"), 8); // Texture unit 8

  //________________Chaser________________
  glActiveTexture(GL_TEXTURE9);
  glGenTextures(1, &chaserTexture);
  glBindTexture(GL_TEXTURE_2D, chaserTexture);
  LoadTGATextureSimple(imagePathChaser, &chaserTexture);
 
  glUniform1i(glGetUniformLocation(*shader, "chaserTexture"), 9); // Texture unit 9

  loadEvaderModels();
  chaserModel = LoadModelPlus(modelPathChaser);

  Evader* evaders = new Evader(cam.position + vec3(25,25,25), 180, 0, cam.position);
  Evader* evaders2 = new Evader(cam.position + vec3(30,30,50), 20, 1, cam.position);

  flocks.push_back(evaders);
  flocks.push_back(evaders2);

  chasers = new Chaser(cam.position + vec3(300,100,300), 3, cam.position);

  prevTime = 0.0;
  modelIndex = 0;
}

ManageChasersAndEvaders::~ManageChasersAndEvaders()
{
  delete chasers;

  for(uint i = 0; i < flocks.size(); i++)
    {
      delete flocks.at(i);
    }
  flocks.clear();

  delete shader;

  // Free/delete chaserModel memory
  glDeleteBuffers(1, &chaserModel->vb);
  glDeleteBuffers(1, &chaserModel->ib);
  glDeleteBuffers(1, &chaserModel->nb);
  glDeleteBuffers(1, &chaserModel->tb);
  glDeleteVertexArrays(1, &chaserModel->vao);
  delete chaserModel;

  // Free/delete evaderModels memory
   for(uint i = 0; i < evaderModels.size(); i++)
    {
      glDeleteBuffers(1, &evaderModels.at(i)->vb);
      glDeleteBuffers(1, &evaderModels.at(i)->ib);
      glDeleteBuffers(1, &evaderModels.at(i)->nb);
      glDeleteBuffers(1, &evaderModels.at(i)->tb);
      glDeleteVertexArrays(1, &evaderModels.at(i)->vao);
      delete evaderModels.at(i);
    }
   evaderModels.clear();

}

void ManageChasersAndEvaders::loadEvaderModels()
{
  char *modelPath = "../objects/evader/crow/crow%d.obj";
  for(int i = 1; i < 42; i++) 
    {
      char file[80]; // Arbitrary length, the string just has to fit
      sprintf(file, modelPath, i);
      
      Model* m = LoadModelPlus(file);
      evaderModels.push_back(m);
    }

}

void vectorAppend(vector<Boid> *v1, vector<Boid> *v2)
{ 
  v1->reserve(v1->size() + v2->size());
  std::move(v2->begin(), v2->end(), std::inserter(*v1, v1->end()));
  v2->clear();
}

// TODO: Should merge the smaller vector into the larger.
// Also: Maybe check if two boids are close instead of the leaders.
// Also: might give vector out of range in some cases! Fix!
void ManageChasersAndEvaders::mergeFlocks(Camera *cam)
{
  float mergeThreshold = 35.0;
  
  for(uint i = 0; i < flocks.size(); i++)
    {
      for(uint j = 0; j < flocks.size(); j++)
	{
	  if(i != j && Norm(flocks.at(j)->leader.position - flocks.at(i)->leader.position) < mergeThreshold && i < j)
	    {
	      if(cam->flockIndex == flocks.at(j)->flockIndex)
		cam->flockIndex = flocks.at(i)->flockIndex;
	      vectorAppend(&flocks.at(i)->evaderVector,&flocks.at(j)->evaderVector);
	      flocks.erase(flocks.begin()+j);
	    }  
	}
    }
}

void ManageChasersAndEvaders::splitFlock(Evader *flock, Camera cam)
{
  float splitDistance = 50.0;
  float minD = 1000.0; // Initial high value
  int minIndex = -1;
  float maxD = 0.0;
  int maxIndex = -1;

  for(uint i = 0; i < flock->evaderVector.size(); i++)
    {
      Boid boidI = flock->evaderVector.at(i);

      if(Norm(flock->leader.position - boidI.position) > maxD)
	{
	  maxD = Norm(flock->leader.position - boidI.position);
	  maxIndex = i;
	}

      if(Norm(flock->leader.position - boidI.position) < minD)
	{
	  minD = Norm(flock->leader.position - boidI.position);
	  minIndex = i;
	}

      //Changing maxD to distance from current maxD to minD.
      maxD = maxD - minD;
    }

  if(maxD > splitDistance && maxIndex != -1 && minIndex != -1)
    {
      vec3 clusterPos1 = flock->evaderVector.at(minIndex).position;
      vec3 clusterPos2 = flock->evaderVector.at(maxIndex).position;
      Evader* evader = new Evader(clusterPos2, 0, (int)flocks.size(), cam.position);
      
      for(uint i = 0; i < flock->evaderVector.size(); i++)
	{
	  vec3 boidIPos = flock->evaderVector.at(i).position;

	  if(Norm(clusterPos2 - boidIPos) < Norm(clusterPos1 - boidIPos))
	    {
	      evader->evaderVector.reserve(evader->evaderVector.size() + 1);
	      std::move(flock->evaderVector.begin() + i,flock->evaderVector.begin() + (i + 1), std::inserter(evader->evaderVector, evader->evaderVector.end()));
	      flock->evaderVector.erase(flock->evaderVector.begin()+i);
	    }
	}

      flocks.push_back(evader);
    }
}

int ManageChasersAndEvaders::nearestFlock(Boid chaser)
{
  float minD = 10000.0; // Initial high value
  int minIndex = -1;

  for(uint i = 0; i < flocks.size(); i++)
    {
      if(Norm(flocks.at(i)->leader.position - chaser.position) < minD)
	{
	  minD = Norm(flocks.at(i)->leader.position - chaser.position);
	  minIndex = i;
	}
    }
  return minIndex;
}

void ManageChasersAndEvaders::sortFlockIndex(Camera *cam)
{
  int numberOfFlocks = flocks.size();
  for(int i = 0; i < numberOfFlocks; i++)
    {
      if(flocks.at(i)->flockIndex != i)
	{
	  if(cam->flockIndex == flocks.at(i)->flockIndex)
	    {
	      cam->flockIndex = i;
	      flocks.at(i)->flockIndex = i;
	    }
	  else
	    flocks.at(i)->flockIndex = i;
	}
    }
}

void ManageChasersAndEvaders::printInfo()
{
  int numberOfFlocks = flocks.size();
  cout << "Number of flocks: " << numberOfFlocks << endl;
  
  for(int i = 0; i < numberOfFlocks; i++)
    {
      cout << "Size of flock i: " << flocks.at(i)->evaderVector.size() << "\n";
      cout << "Flock at index " << i << " has flockIndex: " << flocks.at(i)->flockIndex << endl;
    }
}

void ManageChasersAndEvaders::update(GLfloat time, Camera *cam)
{ 
  // Update flocks (Evader)
  int numberOfFlocks = flocks.size();

  for(int i = 0; i < numberOfFlocks; i++)
    {
      flocks.at(i)->update(time,chasers->chaserVector,*cam);
      splitFlock(flocks.at(i), *cam);
    }

  // Update chasers
  int numberOfChasers = chasers->chaserVector.size();
  //TODO: Fix so that chasers are updated even if they are far from evaders.
  for(int i = 0; i < numberOfChasers; i++)
    {
      int indexNearest = nearestFlock(chasers->chaserVector.at(i));
      if(indexNearest != -1)
	chasers->update(time,i,flocks.at(indexNearest)->evaderVector,*cam);
      else
	cout << "No flock is near chaser " << i << endl;
    }
  
  numberOfFlocks = flocks.size();
  if(numberOfFlocks > 1)
    {
      mergeFlocks(cam);
    }

  sortFlockIndex(cam);
  followCam(cam); 
}

void ManageChasersAndEvaders::followCam(Camera *cam)
{
  if(cam->followFlock)
    {
      if(cam->flockIndex >= (int)flocks.size())
	cam->flockIndex = 0;
      
      cam->lookAtPoint = flocks.at(cam->flockIndex)->position;
      vec3 speedXZ = flocks.at(cam->flockIndex)->speed;
      speedXZ.y = 0.0;
      if(cam->birdView)
	{
	  cam->position = flocks.at(cam->flockIndex)->position - Normalize(speedXZ)*3.0;
	  cam->position.y += 1.0;
	}
      else
	{
	  cam->position = flocks.at(cam->flockIndex)->position - Normalize(speedXZ)*35.0;
	  cam->position.y += 10.0; // So we se the birds from above.
	}

      // // Follow chaser 0. Warning: Will cause seg. fault when the eagle is far from the
      //                     flocks because getActualHeight (calcHeight) will be outside the terrain.
      // vec3 speedChaserXZ = chasers->chaserVector.at(0).speed;
      // speedChaserXZ.y = 0.0;
      // cam->lookAtPoint = chasers->chaserVector.at(0).position;
      // cam->position = chasers->chaserVector.at(0).position - Normalize(speedChaserXZ)*3.0;
      // cam->position.y += 0.5;
    }
}

// void ManageChasersAndEvaders::animate(GLfloat time)
// {
//   if((time - prevTime) > 0.010) // 0.010
//     {
//       if(modelIndex > 40)
// 	modelIndex = 0;
//       for(uint i = 0; i < flocks.size(); i++)
// 	{
// 	  flocks.at(i)->model = evaderModels.at(modelIndex);
// 	}
//       prevTime = time;
//       modelIndex++;
//     }
// }

void ManageChasersAndEvaders::animateAndDraw(GLfloat time, Camera cam)
{
  uint outside = 0;
  if((time - prevTime) > 0.010) // 0.010
    {
      for(uint i = 0; i < flocks.size(); i++)
	{
	  uint numberOfFlockMembers = flocks.at(i)->evaderVector.size();
	  for(uint j = 0; j < numberOfFlockMembers; j++)
	    {
	      uint currentAnimationIndex = flocks.at(i)->evaderVector.at(j).animationIndex;
	      if(currentAnimationIndex > 40)
		flocks.at(i)->evaderVector.at(j).animationIndex = 0;
	      
	      if(cam.isInFrustum(&flocks.at(i)->evaderVector.at(j)))
		flocks.at(i)->evaderVector.at(j).draw(cam.cameraMatrix,shader,evaderModels.at(flocks.at(i)->evaderVector.at(j).animationIndex),&evaderTexture);
	      else
		outside++;
	      flocks.at(i)->evaderVector.at(j).animationIndex++;
	    }
	}
      prevTime = time;
    }
  else
    for(uint i = 0; i < flocks.size(); i++)
      {
  	uint numberOfFlockMembers = flocks.at(i)->evaderVector.size();
  	for(uint j = 0; j < numberOfFlockMembers; j++)
  	  {
  	    uint currentAnimationIndex = flocks.at(i)->evaderVector.at(j).animationIndex;
  	    if(currentAnimationIndex > 40)
  	      flocks.at(i)->evaderVector.at(j).animationIndex = 0;

	    if(cam.isInFrustum(&flocks.at(i)->evaderVector.at(j)))
	      flocks.at(i)->evaderVector.at(j).draw(cam.cameraMatrix,shader,evaderModels.at(flocks.at(i)->evaderVector.at(j).animationIndex),&evaderTexture);
  	  }
  	//prevTime = time;
      }

  cout << "Number of evaders outside frustum: " << outside << endl;
}

void ManageChasersAndEvaders::drawChasers(Camera cam)
{
  uint outside = 0;
  for(uint i = 0; i < chasers->chaserVector.size(); i++)
    {
      if(cam.isInFrustum(&chasers->chaserVector.at(i)))
	chasers->chaserVector.at(i).draw(cam.cameraMatrix,shader,chaserModel,&chaserTexture);
      else
	outside++;
    }
  cout << "Number of chasers outside frustum: " << outside << endl;
}

void ManageChasersAndEvaders::draw(GLfloat time, Camera cam)
{
  glUseProgram(*shader);
  glUniform1i(glGetUniformLocation(*shader, "evader"), 1);
  animateAndDraw(time,cam);
  glUniform1i(glGetUniformLocation(*shader, "evader"), 0);
  drawChasers(cam);
}
