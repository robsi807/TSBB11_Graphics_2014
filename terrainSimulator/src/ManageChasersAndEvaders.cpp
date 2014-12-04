
#include "ManageChasersAndEvaders.h"

// int endWithError(char* msg, int error=0)
// {
//   cout << "Error message: " << msg << endl;
//   //system("PAUSE");
//   return error;

// }

ManageChasersAndEvaders::ManageChasersAndEvaders(GLuint* phongShader, char *modelPathEvader, char *imagePathEvader, char *modelPathChaser, char *imagePathChaser, Camera cam)
{
  shader = phongShader;

  glUseProgram(*shader);

  //________________Evader________________
  glActiveTexture(GL_TEXTURE7);
  //glActiveTexture(GL_TEXTURE0+7);
  //glGenTextures(1, &evaderTexture);
  //glBindTexture(GL_TEXTURE_2D, evaderTexture);
  LoadTGATextureSimple(imagePathEvader, &evaderTexture);
  
  glUniform1i(glGetUniformLocation(*shader, "tex"), 7); // Texture unit 7

  //________________Chaser________________
  glActiveTexture(GL_TEXTURE8);
  //glActiveTexture(GL_TEXTURE0+8);
  //glGenTextures(1, &chaserTexture);
  //glBindTexture(GL_TEXTURE_2D, chaserTexture);
  LoadTGATextureSimple(imagePathChaser, &chaserTexture);
 
  glUniform1i(glGetUniformLocation(*shader, "tex"), 8); // Texture unit 8

  //evaderModel = LoadModelPlus(modelPathEvader);

  loadEvaderModels();
  chaserModel = LoadModelPlus(modelPathChaser);

  Evader* evaders = new Evader(shader, evaderModels.at(0), evaderTexture, cam.position + vec3(25,25,25), 180, 0, cam.position);
  Evader* evaders2 = new Evader(shader, evaderModels.at(0), evaderTexture, cam.position + vec3(30,30,50), 20, 1, cam.position);
  //Evader* evaders = new Evader(shader, "../objects/crowMedium.obj", "../textures/crow.tga", vec3(25,25,25), 150);
  //Evader* evaders2 = new Evader(shader, "../objects/crowMedium.obj", "../textures/crow.tga", vec3(25,25,25), 20);
  flocks.push_back(evaders);
  flocks.push_back(evaders2);

  chasers = new Chaser(shader, chaserModel, chaserTexture, cam.position + vec3(300,100,300), 3, cam.position);
  //chasers = new Chaser(shader, "../objects/eagle.obj", "../textures/eagleBrown.tga", vec3(300,100,300), 3);

  prevTime = 0.0;
  modelIndex = 0;

  birdView = true;

  // fp = NULL;
  // fp=fopen("country_sounds.wav", "rb");
}

// "../textures/skybox/skybox2/sky%d.tga"
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

/*ManageChasersAndEvaders::~ManageChasersAndEvaders()
{

}*/

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
  //int numberOfFlocks = flocks.size();
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
	      cout << "Append!" << endl;
	      flocks.erase(flocks.begin()+j);
		//delete flocks.at(j);
	      cout << "FLOCKS = " << flocks.size() << endl;
	      //return true;
	    }
	  
	}
    }
  //return false;
}

void ManageChasersAndEvaders::splitFlock(Evader *flock, Camera cam)
{
  float splitDistance = 50.0;
  float minD = 1000.0; // Initial high value
  int minIndex = -1;
  float maxD = 0.0;
  int maxIndex = -1;

  //int numberOfEvaders = flock->evaderVector.size();
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

      //Change maxD to distance from current maxD to minD.
      maxD = maxD - minD;
    }

  if(maxD > splitDistance && maxIndex != -1 && minIndex != -1)
    {
      vec3 clusterPos1 = flock->evaderVector.at(minIndex).position;
      vec3 clusterPos2 = flock->evaderVector.at(maxIndex).position;
      Evader* evader = new Evader(shader, evaderModels.at(0), evaderTexture, clusterPos2, 0, (int)flocks.size(), cam.position);
      
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
  //int vectorIndex = -1;
  int numberOfFlocks = flocks.size();
  for(int i = 0; i < numberOfFlocks; i++)
    {
      if(flocks.at(i)->flockIndex != i)
	{
	  // if(cam->flockIndex < flocks.at(i)->flockIndex)
	  //   flocks.at(i)->flockIndex = i;
	  if(cam->flockIndex == flocks.at(i)->flockIndex)
	    {
	      cam->flockIndex = i;
	      flocks.at(i)->flockIndex = i;
	    }
	  else
	    flocks.at(i)->flockIndex = i;
	}
    }
  //return vectorIndex;
}

void ManageChasersAndEvaders::update(GLfloat time, Camera *cam)
{
  int numberOfFlocks = flocks.size();
  //cout << "Number of flocks: " << numberOfFlocks << endl;

  //Test
for(int i = 0; i < numberOfFlocks; i++)
    {
      cout << "Flock at index " << i << " has flockIndex: " << flocks.at(i)->flockIndex << endl;
    }
  
  for(int i = 0; i < numberOfFlocks; i++)
    {
      flocks.at(i)->update(time,chasers->chaserVector,cam->position);
      splitFlock(flocks.at(i), *cam);
      //cout << "Size of flock i: " << flocks.at(i)->evaderVector.size() << "\n";
    }

  int numberOfChasers = chasers->chaserVector.size();
  for(int i = 0; i < numberOfChasers; i++)
    {
      int indexNearest = nearestFlock(chasers->chaserVector.at(i));
      if(indexNearest != -1)
	chasers->update(time,i,flocks.at(indexNearest)->evaderVector,cam->position);
      else
	cout << "No flock is near chaser " << i << endl;
    }
  
  numberOfFlocks = flocks.size();
  if(numberOfFlocks > 1)
    {
      mergeFlocks(cam);
    }

  sortFlockIndex(cam);
  //cout << "Cam: flockIndex = " << cam->flockIndex << endl;
  if(cam->followFlock)
    {
      if(cam->flockIndex >= (int)flocks.size())
	cam->flockIndex = 0;
      
      cam->lookAtPoint = flocks.at(cam->flockIndex)->evaderVector.at(0).averagePosition;
      vec3 speedXZ = flocks.at(cam->flockIndex)->evaderVector.at(0).speed;
      speedXZ.y = 0.0;
      if(birdView)
	{
	  cam->position = flocks.at(cam->flockIndex)->evaderVector.at(0).averagePosition - Normalize(speedXZ)*3.0;
	  cam->position.y += 2.0;
	}
      else
	{
	  cam->position = flocks.at(cam->flockIndex)->evaderVector.at(0).averagePosition - Normalize(speedXZ)*35.0;
	  cam->position.y += 10.0; // So we se the birds from above.
	}

      // camSpeed += flocks.at(cam->flockIndex)->evaderVector.at(0).averagePosition - Normalize(flocks.at(cam->flockIndex)->evaderVector.at(0).speed)*30;
      // if(Norm(camSpeed) > Norm(vec3(1.5,1.5,1.5)))
      // 	camSpeed /= 2.0;
      // cam->position = camSpeed*0.7;

      // // Follow chaser 0
      // vec3 speedChaserXZ = chasers->chaserVector.at(0).speed;
      // speedChaserXZ.y = 0.0;
      // cam->lookAtPoint = chasers->chaserVector.at(0).position;
      // cam->position = chasers->chaserVector.at(0).position - Normalize(speedChaserXZ)*3.0;
      // cam->position.y += 2.0;
    }

  // // Test
  // for(uint i = 0; i < flocks.size(); i++)
  //   {
  //     vec3 avPos = flocks.at(i)->evaderVector.at(0).averagePosition;
  //     cout << "Average pos for flock " << i << " is (" << avPos.x << "," << avPos.y << "," << avPos.z << ")" << endl;
  //   }

  //animate(time);
}

void ManageChasersAndEvaders::animate(GLfloat time)
{
  if((time - prevTime) > 0.010) // 0.010
    {
      if(modelIndex > 40)
	modelIndex = 0;
      for(uint i = 0; i < flocks.size(); i++)
	{
	  flocks.at(i)->model = evaderModels.at(modelIndex);
	}
      prevTime = time;
      modelIndex++;
    }
}

void ManageChasersAndEvaders::animateAndDraw(GLfloat time, mat4 cameraMatrix)
{
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
	      
	      flocks.at(i)->evaderVector.at(j).draw(cameraMatrix,shader,evaderModels.at(flocks.at(i)->evaderVector.at(j).animationIndex),&evaderTexture);
	      flocks.at(i)->evaderVector.at(j).animationIndex++;
	      //cout << "Animation index = " << flocks.at(i)->evaderVector.at(j).animationIndex << endl;
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
  	    flocks.at(i)->evaderVector.at(j).draw(cameraMatrix,shader,evaderModels.at(flocks.at(i)->evaderVector.at(j).animationIndex),&evaderTexture);
  	  }
  	//prevTime = time;
      }
}

void ManageChasersAndEvaders::draw(GLfloat time, mat4 cameraMatrix)
{
  // int N = flocks.size();
  // for(int i = 0; i < N; i++)
  //   {
  //     flocks.at(i)->draw(cameraMatrix);
  //   }
  //glActiveTexture(GL_TEXTURE0+7);
  //glBindTexture(GL_TEXTURE_2D, evaderTexture);
  animateAndDraw(time,cameraMatrix);
  //glActiveTexture(GL_TEXTURE0+8);
  //glBindTexture(GL_TEXTURE_2D, chaserTexture);
  chasers->draw(cameraMatrix);
}
