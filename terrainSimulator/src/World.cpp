#include "World.h"

World::World(){
#if LOWGRAPHICS == 1
    distanceFogConstant = 0.002;
#else
    distanceFogConstant = 0.0005;
#endif
  time = 0;
  specularExponent = 2.0;
  updatingWorld = false;

  patchOverlap = PATCH_OVERLAP;
  patchSize = PATCH_SIZE;
  gridSize = GRID_BEGIN_SIZE;

  // Load shaders
  terrainShader = loadShaders("shaders/terrain.vert","shaders/terrain.frag");
  //terrainShader = loadShadersG("shaders/grass.vert","shaders/grass.frag","shaders/passthrough.gs");
  phongShader = loadShaders("shaders/phong.vert", "shaders/phong.frag");
  skyboxShader = loadShaders("shaders/skybox.vert", "shaders/skybox.frag");
  plantShader = loadShadersG("shaders/plant.vert","shaders/plant.frag","shaders/plant.gs");
#if GRASS == 1
  grassShader = loadShadersG("shaders/grass.vert","shaders/grass.frag","shaders/grass.gs");
#endif
  
  // Init objects
  camera = new Camera(vec3(0.0,60,0.0), 1, 7,&terrainVector, patchSize, patchOverlap,gridSize);

  skybox = new Skybox(&skyboxShader, camera->projectionMatrix, "../textures/skybox/skybox2/sky%d.tga");
  blender = new Blender(patchOverlap);

  sphere = LoadModelPlus("../objects/groundsphere.obj");

  // Init light to terrain shader
  glUseProgram(terrainShader);

  vec3 lightDir = Normalize(vec3(0.0f, 2.0f, 1.0f));
  glUniform3fv(glGetUniformLocation(terrainShader, "lightDirection"), 1, &lightDir.x);
  glUniform1fv(glGetUniformLocation(terrainShader, "specularExponent"), 1, &specularExponent);
  glUniform1fv(glGetUniformLocation(terrainShader, "distanceFogConstant"), 1, &distanceFogConstant);
  glUniformMatrix4fv(glGetUniformLocation(terrainShader, "projMatrix"), 1, GL_TRUE, camera->projectionMatrix.m);
  
#if GRASS == 1
  glUseProgram(grassShader);
  glUniform3fv(glGetUniformLocation(grassShader, "lightDirection"), 1, &lightDir.x);
  glUniform1fv(glGetUniformLocation(grassShader, "specularExponent"), 1, &specularExponent);
  glUniform1fv(glGetUniformLocation(grassShader, "distanceFogConstant"), 1, &distanceFogConstant);
  glUniformMatrix4fv(glGetUniformLocation(grassShader, "projMatrix"), 1, GL_TRUE, camera->projectionMatrix.m);  
#endif

  glUseProgram(phongShader);
  glUniform3fv(glGetUniformLocation(phongShader, "lightDirection"), 1, &lightDir.x);
  glUniform1fv(glGetUniformLocation(phongShader, "specularExponent"), 1, &specularExponent);
  glUniform1fv(glGetUniformLocation(phongShader, "distanceFogConstant"), 1, &distanceFogConstant);
  glUniformMatrix4fv(glGetUniformLocation(phongShader, "projMatrix"), 1, GL_TRUE, camera->projectionMatrix.m);  
  
  // Upload textures to terrain shader
  glUseProgram(terrainShader);
  glActiveTexture(GL_TEXTURE0);
  LoadTGATextureSimple("../textures/grass4_1024_lp.tga", &grassTex1);
  glUniform1i(glGetUniformLocation(terrainShader, "grassTex"), 0); 
  
  glActiveTexture(GL_TEXTURE0+1);
  LoadTGATextureSimple("../textures/fft-terrain.tga", &noiseTex);
  glUniform1i(glGetUniformLocation(terrainShader, "noiseTex"), 1); 

  glActiveTexture(GL_TEXTURE0+2);
  LoadTGATextureSimple("../textures/rock2_1024.tga", &rockTex1);
  glUniform1i(glGetUniformLocation(terrainShader, "rockTex1"), 2);
  glActiveTexture(GL_TEXTURE0+3);
  LoadTGATextureSimple("../textures/rock3_1024.tga", &rockTex2);
  glUniform1i(glGetUniformLocation(terrainShader, "rockTex2"), 3); 

  // Upload textures to grass shader
#if GRASS == 1
  glUseProgram(grassShader);
  glActiveTexture(GL_TEXTURE0+4);
  LoadTGATextureSimple("../textures/grass4_1024_lp2.tga",&grassTexLowPass);
  glUniform1i(glGetUniformLocation(grassShader,"grassTex"),4);

  glActiveTexture(GL_TEXTURE0+5);
  LoadTGATextureSimple("../textures/noise/uniformNoise1.tga",&noiseTex2);
  glUniform1i(glGetUniformLocation(grassShader,"noiseTex"),5);
  
  glActiveTexture(GL_TEXTURE0+6);
  LoadTGATextureSimple("../textures/grass_billboard2.tga",&grassBillboardTex);
  glUniform1i(glGetUniformLocation(grassShader,"grassBillboard"),6);
#endif

  // Loading of plant model, and shader uploads and initialization of plants
#if PLANTS==1  
  Plant::initPlants(&phongShader,&plantShader,&plantShader);
 
  glUseProgram(plantShader);
  glUniform3fv(glGetUniformLocation(plantShader, "lightDirection"), 1, &lightDir.x);
  glUniform1fv(glGetUniformLocation(plantShader, "specularExponent"), 1, &specularExponent);
  glUniform1fv(glGetUniformLocation(plantShader, "distanceFogConstant"), 1, &distanceFogConstant);
  glUniformMatrix4fv(glGetUniformLocation(plantShader, "projMatrix"), 1, GL_TRUE, camera->projectionMatrix.m);
  
  glActiveTexture(GL_TEXTURE0+4);
  glUniform1i(glGetUniformLocation(plantShader,"grassTex"),4);
  glActiveTexture(GL_TEXTURE0+5);
  glUniform1i(glGetUniformLocation(plantShader,"noiseTex"),5);
#endif  

  glUseProgram(phongShader);
  // Upload textures to phong shader
  glActiveTexture(GL_TEXTURE0+2);
  glUniform1i(glGetUniformLocation(phongShader, "tex"), 2);

  generateStartingPatches(GRID_BEGIN_SIZE);
}

void World::generateStartingPatches(int startSize){

  vector<thread> threadVector;
  mutex rowLock; 
  int index;  

  // Initiate height maps
  for(int y = -(startSize-1)/2; y <= (startSize-1)/2; y++){
    vector<TerrainPatch*> terrainRow;
    terrainRow.assign(startSize,NULL);
    index = 0;
    for(int x = -(startSize-1)/2; x <= (startSize-1)/2; x++){
      printf("Adding patch @ %i, %i\n", x, y);
      
      threadVector.push_back(thread(threadGeneratePatch,this,x,y,&terrainRow,&rowLock,index)); 
      index++;
     }
     
     for(unsigned int i=0;i<threadVector.size();i++){
      threadVector.at(i).join();
     }
     
    terrainVector.push_back(terrainRow);
    threadVector.clear();
  }
  
  blender->blendAll(&terrainVector);
	
	// Output height maps to file
	//printTerrainToFile(startSize);

  // Generate geometry for all but the edge patches
  for(int y = 1; y < startSize-1; y++){
    for(int x = 1; x < startSize-1; x++){
      terrainVector.at(y).at(x)->generateAndUploadGeometry();
      //printf("Generating geometry @ %i, %i\n", terrainVector.at(y).at(x)->xGrid, terrainVector.at(y).at(x)->yGrid);
    }
  }
}

void World::printTerrainToFile(int startSize){
	cout << "Printing to file ...\n";
	fstream myfile;
	stringstream sstream;
	string fileName;
	int size = terrainVector.at(0).at(0)->size;
	sstream << "../../Evaluation/raw_" << size << "_heightmap.m";
	fileName = sstream.str();
	myfile.open(fileName, fstream::out);
	
	for(int y = 0; y < startSize; y++){
    for(int x = 0; x < startSize; x++){
			TerrainPatch* patch = terrainVector.at(y).at(x);
			vector<float> rawHeightMap = patch->rawHeightMap;
  
			myfile << "patch_"<< x << "_" << y << " = [";

			for(int row = 0; row < size; row++) {
				for(int col = 0; col < size; col++) {
					myfile << rawHeightMap.at(row*size + col) << ",";
				} 
				myfile << ";\n";
			} 
    	myfile << "];\n\n";
    } 
  }
 
	myfile.close();
	cout << "Printing to file done.\n";

}

void World::addTerrainSouth() {
  
  vector<thread> threadVector;
  mutex rowLock; 
  int index = 0;  
  
  int xSize = terrainVector.at(0).size();
  int ySize = terrainVector.size();
  vector<TerrainPatch*> newTerrainVec;
  newTerrainVec.assign(xSize,NULL);

  // Calculate new coordinates in grid
  int yGrid = terrainVector.at(0).at(0)->yGrid - 1;
  int xGridBegin = terrainVector.at(ySize-1).at(0)->xGrid;
  for(int x = 0; x < xSize; x++){
    int xGrid = xGridBegin + x;
    threadVector.push_back(thread(threadGeneratePatch,this,xGrid,yGrid,&newTerrainVec,&rowLock,index)); 
    index++;
  }
  
  for(unsigned int i=0;i<threadVector.size();i++){
     threadVector.at(i).join();
  }
  
  terrainMutex.lock();
  terrainVector.insert(terrainVector.begin(),newTerrainVec);
	terrainMutex.unlock();
	
  threadVector.clear();

  blender->blendSouth(&terrainVector);
  
  for(int x = 1; x < xSize-1; x++){
    terrainVector.at(1).at(x)->generateAndUploadGeometry();
  }
  //printf("Terrain added south at yGrid = %i.\n",yGrid);

}

void World::addTerrainNorth() {

  vector<thread> threadVector;
  mutex rowLock; 
  int index = 0;  

  int xSize = terrainVector.at(0).size();
  int ySize = terrainVector.size();
  vector<TerrainPatch*> newTerrainVec;
  newTerrainVec.assign(xSize,NULL);

  // Calculate new coordinates in grid
  int yGrid = terrainVector.at(ySize-1).at(0)->yGrid + 1;
  int xGridBegin = terrainVector.at(ySize-1).at(0)->xGrid;
  for(int x = 0; x < xSize; x++){
    int xGrid = xGridBegin + x;
    threadVector.push_back(thread(threadGeneratePatch,this,xGrid,yGrid,&newTerrainVec,&rowLock,index)); 
    index++;
  }
  
  for(unsigned int i=0;i<threadVector.size();i++){
     threadVector.at(i).join();
  }
	
	terrainMutex.lock();
  terrainVector.push_back(newTerrainVec);
	terrainMutex.unlock();

  threadVector.clear();

  blender->blendNorth(&terrainVector);

  for(int x = 1; x < xSize-1; x++){
    terrainVector.at(ySize-1).at(x)->generateAndUploadGeometry();
    //printf("Generating geometry @ %i, %i\n", terrainVector.at(ySize-1).at(x)->xGrid, terrainVector.at(ySize-1).at(x)->yGrid);
  }
  //printf("Terrain added north at yGrid = %i.\n",yGrid);  
}

void World::addTerrainEast(){
  
  vector<thread> threadVector;
  mutex rowLock; 
  int index = 0;  
  
  int xSize = terrainVector.at(0).size();
  int ySize = terrainVector.size();
  vector<TerrainPatch*> newTerrainVec;
  newTerrainVec.assign(ySize,NULL);

  // Calculate new coordinates in grid
  int yGridBegin = terrainVector.at(0).at(0)->yGrid;
  int xGrid = terrainVector.at(ySize-1).at(0)->xGrid - 1;

  for(int y = 0; y < ySize; y++){
    int yGrid = yGridBegin + y;
    
    threadVector.push_back(thread(threadGeneratePatch,this,xGrid,yGrid,&newTerrainVec,&rowLock,index)); 
    index++;
  }
  
  for(unsigned int i=0;i<threadVector.size();i++){
     threadVector.at(i).join();
  }
  
  terrainMutex.lock();
  for(int y = 0; y < ySize; y++){
    terrainVector.at(y).insert(terrainVector.at(y).begin(),newTerrainVec.at(y));
  }
	terrainMutex.unlock();

  threadVector.clear();

  blender->blendEast(&terrainVector);

  for(int y = 1; y < ySize-1; y++){
    terrainVector.at(y).at(1)->generateAndUploadGeometry();
  }
}

void World::addTerrainWest(){
  
  vector<thread> threadVector;
  mutex rowLock; 
  int index = 0;  
  
  int xSize = terrainVector.at(0).size();
  int ySize = terrainVector.size();
  vector<TerrainPatch*> newTerrainVec;
  newTerrainVec.assign(ySize,NULL);

  // Calculate new coordinates in grid
  int yGridBegin = terrainVector.at(0).at(0)->yGrid;
  int xGrid = terrainVector.at(ySize-1).at(xSize-1)->xGrid + 1;

  for(int y = 0; y < ySize; y++){
    int yGrid = yGridBegin + y;
    
    threadVector.push_back(thread(threadGeneratePatch,this,xGrid,yGrid,&newTerrainVec,&rowLock,index)); 
    index++;
  }
  
  for(unsigned int i=0;i<threadVector.size();i++){
     threadVector.at(i).join();
  }
  
  terrainMutex.lock();
  for(int y = 0; y < ySize; y++){
    terrainVector.at(y).push_back(newTerrainVec.at(y));
  }
  terrainMutex.unlock();
  
  threadVector.clear();
  
  blender->blendWest(&terrainVector);

  for(int y = 1; y < ySize-1; y++){
    terrainVector.at(y).at(xSize-1)->generateAndUploadGeometry();
  }
  //printf("Terrain added west at xGrid = %i\n",xGrid);

}

void World::removeTerrainSouth() {
  for (int x = 0; x < terrainVector.at(0).size();x++) {
    if(terrainVector.at(0).at(x) != NULL)
      delete terrainVector.at(0).at(x);
  }
    terrainVector.erase(terrainVector.begin());
};

void World::removeTerrainNorth() {
  int ySize = terrainVector.size();
  for (int x = 0; x < terrainVector.at(ySize-1).size();x++) {
    if(terrainVector.at(ySize-1).at(x) != NULL)
      delete terrainVector.at(ySize-1).at(x);
  }
  terrainVector.erase(terrainVector.end()-1);
};

void World::removeTerrainEast() {
  for (int y = 0; y < terrainVector.size();y++) {
    if(terrainVector.at(y).at(0) != NULL)
      delete terrainVector.at(y).at(0);
    terrainVector.at(y).erase(terrainVector.at(y).begin());
  }

};

void World::removeTerrainWest() {
  for (int y = 0; y < terrainVector.size();y++) {
    if(terrainVector.at(y).at(terrainVector.at(y).size() - 1) != NULL)
      delete terrainVector.at(y).at(terrainVector.at(y).size() - 1);
    terrainVector.at(y).erase(terrainVector.at(y).begin() + terrainVector.at(y).size() - 1);
  }
};


TerrainPatch* World::generatePatch(int patchX, int patchY){

  return new TerrainPatch(patchSize, patchX, patchY,patchOverlap, &terrainShader, &grassShader);

}

void World::update(){
  time = (GLfloat)glutGet(GLUT_ELAPSED_TIME)/1000;

  updateTerrain();
  camera->update();
}

void World::draw(){

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  skybox->draw(camera->cameraMatrix);

	terrainMutex.lock();
	
	for(int y = 0; y < terrainVector.size(); y++){
    for(int x = 0; x < terrainVector.at(y).size(); x++){
      TerrainPatch *patch = terrainVector.at(y).at(x);
      if(patch != NULL){
	      patch->draw(camera,time);
      }
    }
  }

	terrainMutex.unlock();
/*
  if(terrainMutex.try_lock()){

    for(int y = 0; y < terrainVector.size(); y++){
      for(int x = 0; x < terrainVector.at(y).size(); x++){
        TerrainPatch *patch = terrainVector.at(y).at(x);
        if(camera->isInFrustum(patch) && patch->hasGeometry()){
	        patch->draw(camera,time);
        }
      }
    }
    terrainMutex.unlock();
    
    } else {
     for(int y = 1; y < terrainVector.size()-1; y++){
      for(int x = 1; x < terrainVector.at(y).size()-1; x++){
        TerrainPatch *patch = terrainVector.at(y).at(x);
        if(camera->isInFrustum(patch) && patch->hasGeometry()){
	        patch->draw(camera,time);
        }
      }
    }
   }
    */
  //terrainMutex.unlock();
  
}

void World::updateTerrain(){

  if(!updatingWorld) {

    int yIndex = (terrainVector.size() - 1) / 2 + 0;
    int xIndex = (terrainVector.at(yIndex).size() - 1) / 2 + 0;
    
    TerrainPatch* middlePatch = terrainVector.at(yIndex).at(xIndex);
    
    int patchSize = middlePatch->size - middlePatch->patchOverlap;
    int xPatch = middlePatch-> xPos;
    int yPatch = middlePatch-> yPos;

    int xCam = camera->getPosition().x;
    int yCam = camera->getPosition().z; // well... we go from 3D to 2D.

    if(xCam < xPatch) { // move EAST.
        updatingWorld = true;
        thread threadEast(threadAddPatchEast, this);
        threadEast.detach();
    } else if (xCam > xPatch + patchSize) { // move WEST
        
        updatingWorld = true;
        thread threadWest(threadAddPatchWest, this);
        threadWest.detach(); 
    }
    
    else if(yCam < yPatch) { // move SOUTH.
        updatingWorld = true;
        thread threadSouth(threadAddPatchSouth, this);
        threadSouth.detach();
    } else if (yCam > yPatch + patchSize) { // move NORTH
        updatingWorld = true;
        thread threadNorth(threadAddPatchNorth, this);
        threadNorth.detach();
    }

  }

}

World::~World(){
  delete camera;
  delete skybox;
  delete blender;
  terrainVector.clear();
}

// Threading functions
void threadGeneratePatch(World *w, int x, int y, vector<TerrainPatch*> *terrainRow, std::mutex * lock, int index)
{
  //cout << "Index is: " << index << endl;
  TerrainPatch* newPatch = w->generatePatch(x,y);
  lock->lock();
  //  terrainRow->insert(terrainRow->begin()+index, newPatch);
  terrainRow->at(index) = newPatch;
  lock->unlock();
}

void threadAddPatchNorth(World *w){
  makeWorkerCurrent();
    
  w->addTerrainNorth();
  w->terrainMutex.lock();
  w->removeTerrainSouth();
  w->terrainMutex.unlock();
  
  makeMainContextCurrent();
  w->updatingWorld = false;
}

void threadAddPatchSouth(World *w){
  makeWorkerCurrent();
  
  w->addTerrainSouth();
  w->terrainMutex.lock();
  w->removeTerrainNorth();
  w->terrainMutex.unlock();
  
  makeMainContextCurrent();
  w->updatingWorld = false; 
}

void threadAddPatchWest(World *w){
  makeWorkerCurrent();
  
  w->addTerrainWest();
  w->terrainMutex.lock();
  w->removeTerrainEast();
  w->terrainMutex.unlock();
  
  makeMainContextCurrent();
  w->updatingWorld = false;
}

void threadAddPatchEast(World *w){
  makeWorkerCurrent();
    
  w->addTerrainEast();
  w->terrainMutex.lock();
  w->removeTerrainWest();
  w->terrainMutex.unlock();
  
  makeMainContextCurrent();
  w->updatingWorld = false;
}
