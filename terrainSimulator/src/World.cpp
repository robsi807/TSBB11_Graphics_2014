#include "World.h"

World::World(){
	
  time = 0;
  
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
  int biotope = 1; // 1 = mountains, 2 = desert
  int NoF = 7; // Number of frequencies, 1 <= NoF <= 9. Standard = 9. Max value on n: 2^n <= size
  int amplitude = 1; //Scalefactor for the amplitude. Standard = 1.
  //patchGenerator = new PerlinPatchGenerator(biotope, NoF, amplitude, patchSize);
  patchGenerator = new ValuePatchGenerator(biotope, NoF, amplitude, patchSize);
  //patchGenerator = new DebugPatchGenerator(false);

  camera = new Camera(vec3(patchSize/2.0,60,patchSize/2.0), 1, 7);
  skybox = new Skybox(&skyboxShader, camera->projectionMatrix, "../textures/skybox/skybox2/sky%d.tga");
  blender = new LinearBlender(patchOverlap);

  sphere = LoadModelPlus("../objects/groundsphere.obj");

  // Init light to terrain shader
  glUseProgram(terrainShader);

  vec3 lightDir = Normalize(vec3(0.0f, 2.0f, 1.0f));
  GLfloat specularExponent = 2.0;
  glUniform3fv(glGetUniformLocation(terrainShader, "lightDirection"), 1, &lightDir.x);
  glUniform1fv(glGetUniformLocation(terrainShader, "specularExponent"), 1, &specularExponent);
  glUniformMatrix4fv(glGetUniformLocation(terrainShader, "projMatrix"), 1, GL_TRUE, camera->projectionMatrix.m);
  
#if GRASS == 1
  glUseProgram(grassShader);
  glUniform3fv(glGetUniformLocation(grassShader, "lightDirection"), 1, &lightDir.x);
  glUniform1fv(glGetUniformLocation(grassShader, "specularExponent"), 1, &specularExponent);
  glUniformMatrix4fv(glGetUniformLocation(grassShader, "projMatrix"), 1, GL_TRUE, camera->projectionMatrix.m);  
#endif

  glUseProgram(phongShader);
  glUniform3fv(glGetUniformLocation(phongShader, "lightDirection"), 1, &lightDir.x);
  glUniform1fv(glGetUniformLocation(phongShader, "specularExponent"), 1, &specularExponent);
  glUniformMatrix4fv(glGetUniformLocation(phongShader, "projMatrix"), 1, GL_TRUE, camera->projectionMatrix.m);  
  
  // Upload textures to terrain shader
  glUseProgram(terrainShader);
  GLuint grassTex1;
  glActiveTexture(GL_TEXTURE0);
  LoadTGATextureSimple("../textures/grass4_1024_lp.tga", &grassTex1);
  glUniform1i(glGetUniformLocation(terrainShader, "grassTex"), 0); 
  
  GLuint noiseTex;
  glActiveTexture(GL_TEXTURE0+1);
  LoadTGATextureSimple("../textures/fft-terrain.tga", &noiseTex);
  glUniform1i(glGetUniformLocation(terrainShader, "noiseTex"), 1); 

  GLuint rockTex1;
  glActiveTexture(GL_TEXTURE0+2);
  LoadTGATextureSimple("../textures/rock2_1024.tga", &rockTex1);
  glUniform1i(glGetUniformLocation(terrainShader, "rockTex1"), 2);

  GLuint rockTex2;
  glActiveTexture(GL_TEXTURE0+3);
  LoadTGATextureSimple("../textures/rock3_1024.tga", &rockTex2);
  glUniform1i(glGetUniformLocation(terrainShader, "rockTex2"), 3); 

  // Upload textures to grass shader
#if GRASS == 1
  glUseProgram(grassShader);
  GLuint grassTexLowPass;
  glActiveTexture(GL_TEXTURE0+4);
  LoadTGATextureSimple("../textures/grass4_1024_lp.tga",&grassTexLowPass);
  glUniform1i(glGetUniformLocation(grassShader,"grassTex"),4);

  GLuint noiseTex2;
  glActiveTexture(GL_TEXTURE0+5);
  LoadTGATextureSimple("../textures/noise/uniformNoise1.tga",&noiseTex2);
  glUniform1i(glGetUniformLocation(grassShader,"noiseTex"),5);
#endif

  // Loading of plant model, and shader uploads
  plantModel = LoadModelPlus("../objects/groundsphere.obj");
  plant = new Plant(&phongShader,&plantShader,plantModel,vec3(0,10,0),0.0,5.0);

  glUseProgram(plantShader);
  glUniform3fv(glGetUniformLocation(plantShader, "lightDirection"), 1, &lightDir.x);
  glUniform1fv(glGetUniformLocation(plantShader, "specularExponent"), 1, &specularExponent);
  glUniformMatrix4fv(glGetUniformLocation(plantShader, "projMatrix"), 1, GL_TRUE, camera->projectionMatrix.m);
  
  glActiveTexture(GL_TEXTURE0+4);
  glUniform1i(glGetUniformLocation(plantShader,"grassTex"),4);
  glActiveTexture(GL_TEXTURE0+5);
  glUniform1i(glGetUniformLocation(plantShader,"noiseTex"),5);
  
  glUseProgram(phongShader);
  // Upload textures to phong shader
  glActiveTexture(GL_TEXTURE0+2);
  glUniform1i(glGetUniformLocation(phongShader, "tex"), 2);

  generateStartingPatches(GRID_BEGIN_SIZE);
}

void World::generateStartingPatches(int startSize){

  // Initiate height maps
  for(int y = -(startSize-1)/2; y <= (startSize-1)/2; y++){
    vector<TerrainPatch*> terrainRow;
    for(int x = -(startSize-1)/2; x <= (startSize-1)/2; x++){
      printf("Adding patch @ %i, %i\n", x, y);
      terrainRow.push_back(generatePatch(x,y));
    }
    terrainVector.push_back(terrainRow);
  }

  blender->blendAll(&terrainVector);

  // Generate geometry for all but the edge patches
  for(int y = 1; y < startSize-1; y++){
    for(int x = 1; x < startSize-1; x++){
      terrainVector.at(y).at(x)->generateAndUploadGeometry();
      printf("Generating geometry @ %i, %i\n", terrainVector.at(y).at(x)->xGrid, terrainVector.at(y).at(x)->yGrid);
    }
  }
}

void World::addTerrainSouth() {
  
  int xSize = terrainVector.at(0).size();
  int ySize = terrainVector.size();
  vector<TerrainPatch*> newTerrainVec;

  // Calculate new coordinates in grid
  int yGrid = terrainVector.at(0).at(0)->yGrid - 1;
  int xGridBegin = terrainVector.at(ySize-1).at(0)->xGrid;
  for(int x = 0; x < xSize; x++){
    int xGrid = xGridBegin + x;
    TerrainPatch* newPatch = generatePatch(xGrid,yGrid);
    newTerrainVec.push_back(newPatch);
  }
  terrainVector.insert(terrainVector.begin(),newTerrainVec);


  blender->blendSouth(&terrainVector);

  
  for(int x = 1; x < xSize-1; x++){
    terrainVector.at(1).at(x)->generateAndUploadGeometry();
  }
  printf("Terrain added south at yGrid = %i.\n",yGrid);

}

void World::addTerrainNorth() {

  int xSize = terrainVector.at(0).size();
  int ySize = terrainVector.size();
  vector<TerrainPatch*> newTerrainVec;


  // Calculate new coordinates in grid
  int yGrid = terrainVector.at(ySize-1).at(0)->yGrid + 1;
  int xGridBegin = terrainVector.at(ySize-1).at(0)->xGrid;
  for(int x = 0; x < xSize; x++){
    int xGrid = xGridBegin + x;
    TerrainPatch* newPatch = generatePatch(xGrid,yGrid);
    printf("Adding patch @ %i, %i\n", xGrid, yGrid);
    newTerrainVec.push_back(newPatch);
  }
  terrainVector.push_back(newTerrainVec);



  blender->blendNorth(&terrainVector);


  for(int x = 1; x < xSize-1; x++){
    terrainVector.at(ySize-1).at(x)->generateAndUploadGeometry();
    printf("Generating geometry @ %i, %i\n", terrainVector.at(ySize-1).at(x)->xGrid, terrainVector.at(ySize-1).at(x)->yGrid);
  }
  printf("Terrain added north at yGrid = %i.\n",yGrid);  
}

void World::addTerrainEast(){
  
  int xSize = terrainVector.at(0).size();
  int ySize = terrainVector.size();
  vector<TerrainPatch*> newTerrainVec;

  // Calculate new coordinates in grid
  int yGridBegin = terrainVector.at(0).at(0)->yGrid;
  int xGrid = terrainVector.at(ySize-1).at(0)->xGrid - 1;

  for(int y = 0; y < ySize; y++){
    int yGrid = yGridBegin + y;
    TerrainPatch* newPatch = generatePatch(xGrid,yGrid);
    terrainVector.at(y).insert(terrainVector.at(y).begin(),newPatch);
  }

  blender->blendEast(&terrainVector);

  for(int y = 1; y < ySize-1; y++){
    terrainVector.at(y).at(1)->generateAndUploadGeometry();
  }
}

void World::addTerrainWest(){
  
  int xSize = terrainVector.at(0).size();
  int ySize = terrainVector.size();
  vector<TerrainPatch*> newTerrainVec;

  // Calculate new coordinates in grid
  int yGridBegin = terrainVector.at(0).at(0)->yGrid;
  int xGrid = terrainVector.at(ySize-1).at(xSize-1)->xGrid + 1;

  for(int y = 0; y < ySize; y++){
    int yGrid = yGridBegin + y;
    TerrainPatch* newPatch = generatePatch(xGrid,yGrid);
    terrainVector.at(y).push_back(newPatch);
  }
  
  blender->blendWest(&terrainVector);

  for(int y = 1; y < ySize-1; y++){
    terrainVector.at(y).at(xSize-1)->generateAndUploadGeometry();
  }
  printf("Terrain added east at xGrid = %i\n",xGrid);

}

void World::removeTerrainSouth() {

  terrainVector.erase(terrainVector.begin());
};

void World::removeTerrainNorth() {
  terrainVector.erase(terrainVector.end());
};

void World::removeTerrainEast() {
  for (int y = 0; y < terrainVector.size();y++) {
    terrainVector.at(y).erase(terrainVector.at(y).begin());
  }

};

void World::removeTerrainWest() {
  for (int y = 0; y < terrainVector.size();y++) {
    terrainVector.at(y).erase(terrainVector.at(y).begin() + terrainVector.at(y).size() - 1);
  }
};


TerrainPatch* World::generatePatch(int patchX, int patchY){

  vector<float> heightMapPatch = patchGenerator->generatePatch(patchX, patchY);

  return new TerrainPatch(heightMapPatch,patchSize, patchX, patchY,patchOverlap, &terrainShader, &grassShader);
}


void threadCreatePatch(World *w, int x, int y);


/*
void threadUpdateTerrain(World *w){
  makeWorkerCurrent();
  w->updateTerrain();
  makeMainContextCurrent();
}
*/


void createThreadedPatchRow(World *w, int patchX, int patchY, int xDirection, int yDirection) {
  
  vector<thread> threads;
  
  for (int i = 0;i < w->gridSize;i++) {
    
    
    threads.push_back(thread(threadCreatePatch, w, patchX + i * xDirection, patchY + i * yDirection));
  }
  
  for (int i = 0;i < w->gridSize;i++) {
    threads.at(i).join();
  }
  
}



void threadCreatePatch(World *w, int x, int y) {
  //cout << "threadCreatePatch creating patch...\n";
  TerrainPatch* patch = w->generatePatch(x,y);
  
  w->terrainRowMutex.lock();
  
  w->terrainRow.push_back(patch);
  
  //cout << "threadCreatePatch done creating patch...\n";
  
  w->terrainRowMutex.unlock();
  
   
}




void World::addTerrainNorth2() {

  // Calculate new coordinates in grid
  terrainVector.push_back(terrainRow);

  blender->blendNorth(&terrainVector);

  int ySize = terrainVector.size();
  int xSize = terrainVector.at(0).size();


  makeWorkerCurrent();
  
  for(int x = 1; x < xSize-1; x++){
    terrainVector.at(ySize-2).at(x)->generateAndUploadGeometry(); // magic 2, we generate geometry for the second last, 0 indexed.
    printf("Generating geometry @ %i, %i\n", terrainVector.at(ySize-2).at(x)->xGrid, terrainVector.at(ySize-2).at(x)->yGrid);
  }
  

  makeMainContextCurrent();
  
  
  
  printf("Terrain added north at yGrid = %i.\n",terrainVector.at(ySize-2).at(0)->yGrid+1);
  
  
  
}


void threadAddPatchNorth2(World *w){
  //cout << "threadAPN2 starting!\n";
  
  w->terrainGenerationMutex.lock();
  
  int ySize = w->terrainVector.at(0).size();
  
  int yGrid = w->terrainVector.at(ySize-1).at(0)->yGrid + 1;
  int xGrid = w->terrainVector.at(ySize-1).at(0)->xGrid;
  
  createThreadedPatchRow(w, xGrid, yGrid, 0, +1); // will wait for whole row to generate.
  
  // lock terrainVector.
  
  w->terrainMutex.lock();
  
  // add the row
  // blend the stuff
  // generate geometry
  
  //cout << "threadAPN2 adding terrain in North!\n";
  w->addTerrainNorth2();
  
  //cout << "threadAPN2 removing terrain in south!\n";
  // remove other row
  w->removeTerrainSouth();
  
  
  
  //cout << "threadAPN2 Switching Context!\n";
  // make uploading context
 
  /*
 
  makeWorkerCurrent(); // *******************************================================================================================
  
  // upload to GPU
  
  //cout << "threadAPN2 uploading geometry!!\n";
  for(int i = 0; i < w->terrainRow.size();i++) {
    w->terrainRow.at(i)->uploadGeometry();
  }
  
  
  //cout << "threadAPN2 Switching Context back!!\n";
  
  // unlock context
  makeMainContextCurrent();
  
  */
  
  // unlock terrainVector.
  
  
  //cout << "threadAPN2 unlocking mutexes!!\n";
  
  w->terrainMutex.unlock(); 
     
  w->terrainGenerationMutex.unlock();
  
  
}



void threadAddPatchNorth(World *w){
  //cout << "threadAddPatchNorth working... \n";
  makeWorkerCurrent();
  
  //cout << "ThreadAPN: Trying to acquire mutex...\n";

  
  //w->terrainVectorCopy = w->terrainVector;
  w->terrainMutex.lock();
  //cout << "threadAPNorth: acquired lock! \n";
  
  //cout << "threadAddPatchNorth adding... \n";
  w->addTerrainNorth();
  //cout << "threadAddPatchNorth removing... \n";
  w->removeTerrainSouth();
  makeMainContextCurrent();
  w->updatingWorld = false;
  w->terrainMutex.unlock();
  //cout << "threadAPNorth: released lock! \n";
  
}
void threadAddPatchSouth(World *w){

  //cout << "threadAddPatchSouth working... \n";
  makeWorkerCurrent();
  
  
  //cout << "ThreadAPS: Trying to acquire mutex...\n";
  
  /*
  if(w->terrainMutex.try_lock()) {
    cout << " successful! \n";
  } else {
    cout << "... failed... going to sleep!";
  }
  */
  
  //w->terrainVectorCopy = w->terrainVector;
  w->terrainMutex.lock();
  //cout << "threadAPSouth: acquired lock! \n";

  //cout << "threadAddPatchSouth adding to south!\n";
  w->addTerrainSouth();
  //cout << "threadAddPatchSouth removing from north!\n";
  w->removeTerrainNorth();
  makeMainContextCurrent();
  w->updatingWorld = false;
  w->terrainMutex.unlock();
  //cout << "threadAPSouth: released lock! \n";
  
}
void threadAddPatchWest(World *w){

  //cout << "threadAddPatchWest working... \n";
  makeWorkerCurrent();
  
  //cout << "ThreadAPW: Trying to acquire mutex...\n";

/*  
  if(w->terrainMutex.try_lock()) {
    //cout << " successful! \n";
  } else {
    //cout << "... failed... going to sleep!";
  }
*/
  
  //w->terrainVectorCopy = w->terrainVector;
  
  w->terrainMutex.lock();
  //cout << "threadAPWest: acquired lock! \n";
  
  
  //cout << "threadAddPatchWest adding... \n";
  w->addTerrainWest();
  //cout << "threadAddPatchWest removing... \n";
  w->removeTerrainEast();
  makeMainContextCurrent();
  w->updatingWorld = false;
  w->terrainMutex.unlock();
  //cout << "threadAPWest: released lock! \n";
}
void threadAddPatchEast(World *w){

  //cout << "threadAddPatchEast working... \n";
  makeWorkerCurrent();
  
  //cout << "ThreadAPE: Trying to acquire mutex...\n";
  
  /*
  if(w->terrainMutex.try_lock()) {
    //cout << " successful! \n";
  } else {
    //cout << "... failed... going to sleep!";
  }
  */
  
  //w->terrainVectorCopy = w->terrainVector;
  w->terrainMutex.lock();
  //cout << "threadAPEast: acquired lock! \n";
  
  
  //cout << "threadAddPatchEast adding... \n";
  w->addTerrainEast();
  //cout << "threadAddPatchEast removing... \n";
  w->removeTerrainWest();
  makeMainContextCurrent();
  w->updatingWorld = false;
  w->terrainMutex.unlock();
  //cout << "threadAPEast: released lock! \n";
}

void World::update(){
  time = (GLfloat)glutGet(GLUT_ELAPSED_TIME)/1000;

  updateTerrain();
  camera->update();

   if(camera->addTerrain != 0){

    if(camera->addTerrain == NORTH){
      camera->addTerrain = 0;
      thread threadNorth(threadAddPatchNorth2, this);
      threadNorth.detach();

    }
    else if(camera->addTerrain == SOUTH){
      camera->addTerrain = 0;
      thread threadSouth(threadAddPatchSouth, this);
      threadSouth.detach();

    }
    else if(camera->addTerrain == EAST){
      camera->addTerrain = 0;
      thread threadEast(threadAddPatchEast, this);
      threadEast.detach();

    }
    else if(camera->addTerrain == WEST){
      camera->addTerrain = 0;
      thread threadWest(threadAddPatchWest, this);
      threadWest.detach();
    }
  }
}

void World::draw(){

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  skybox->draw(camera->cameraMatrix);

  if(terrainMutex.try_lock()) {

    for(int y = 0; y < terrainVector.size(); y++){
      for(int x = 0; x < terrainVector.at(y).size(); x++){
        TerrainPatch *patch = terrainVector.at(y).at(x);
        if(camera->isInFrustum(patch) && patch->hasGeometry()){
	  patch->draw(camera->cameraMatrix,time);
        }
      }
    }
    terrainMutex.unlock();
  
  } else {
  
    //cout << "World::draw: SOMEONE HAS LOCKED THE TERRAINS T.T \n";
  
    for(int y = 1; y < gridSize-1; y++){
      for(int x = 1; x < gridSize-1; x++){
        TerrainPatch *patch = terrainVector.at(y).at(x);
        if(camera->isInFrustum(patch) && terrainVector.at(y).at(x)->hasGeometry()){
	  terrainVector.at(y).at(x)->draw(camera->cameraMatrix,time);
        }
      }
    }
  
  }

  plant -> draw(camera,time);
  mat4 modelView = T(0,35,0);
  glUniformMatrix4fv(glGetUniformLocation(terrainShader, "mdl2World"), 1, GL_TRUE, modelView.m);
  glUniformMatrix4fv(glGetUniformLocation(terrainShader, "world2View"), 1, GL_TRUE, camera->cameraMatrix.m);
  DrawModel(sphere, terrainShader, "inPosition", "inNormal","inTexCoord"); 

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
    
    if(yCam < yPatch) { // move SOUTH.
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
  delete patchGenerator;
}
