#include "World.h"

World::World(){
	
  time = 0;
  
  updatingWorld = false;

  patchOverlap = PATCH_OVERLAP;
  patchSize = PATCH_SIZE;
  gridSize = GRID_BEGIN_SIZE;

  // Load shaders
  terrainShader = loadShaders("shaders/terrain.vert","shaders/terrain.frag");
  phongShader = loadShaders("shaders/phong.vert", "shaders/phong.frag");
  skyboxShader = loadShaders("shaders/skybox.vert", "shaders/skybox.frag");

  // Init objects
  //patchGenerator = new PerlinPatchGenerator();
  //patchGenerator = new ValuePatchGenerator();
  patchGenerator = new DebugPatchGenerator();

  camera = new Camera(vec3(0,60,0), 1, 7);
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
  
  // Upload textures to terrain shader
  GLuint grassTex1;
  glActiveTexture(GL_TEXTURE0);
  LoadTGATextureSimple("../textures/grass2_1024.tga", &grassTex1);
  glUniform1i(glGetUniformLocation(terrainShader, "tex1"), 0); 
  
  GLuint grassTex2;
  glActiveTexture(GL_TEXTURE0+1);
  LoadTGATextureSimple("../textures/grass3_1024.tga", &grassTex2);
  glUniform1i(glGetUniformLocation(terrainShader, "tex2"), 1); 

  GLuint rockTex1;
  glActiveTexture(GL_TEXTURE0+2);
  LoadTGATextureSimple("../textures/rock2_1024.tga", &rockTex1);
  glUniform1i(glGetUniformLocation(terrainShader, "tex3"), 2);

  GLuint rockTex2;
  glActiveTexture(GL_TEXTURE0+3);
  LoadTGATextureSimple("../textures/rock3_1024.tga", &rockTex2);
  glUniform1i(glGetUniformLocation(terrainShader, "tex4"), 3); 

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

  // Blend!
  cout << "Blending patches... " << endl;
  for(int y = 0; y < terrainVector.size(); y++){
    for(int x = 0; x < terrainVector.at(y).size(); x++){
      // Blend horizontal
      if(x != 0){
	TerrainPatch *pWest,*pEast;
	pWest = terrainVector.at(y).at(x-1);
	pEast = terrainVector.at(y).at(x);
	blender->blendHors(pWest,pEast);
      }
      // Blend vertical
      if(y != 0){
	TerrainPatch *pSouth,*pNorth;
	pSouth = terrainVector.at(y-1).at(x);
	pNorth = terrainVector.at(y).at(x);
	blender->blendVert(pSouth,pNorth);
      }
      // Blend corners
      if(y != 0 && x != 0){
	TerrainPatch *p00,*p01,*p10,*p11;
	p00 = terrainVector.at(y-1).at(x-1);
	p01 = terrainVector.at(y-1).at(x);
	p10 = terrainVector.at(y).at(x-1);
	p11 = terrainVector.at(y).at(x);
	blender->blendCorners(p00,p01,p10,p11);
      }    
    }
  }

  // Generate geometry for all but the edge patches
  for(int y = 1; y < startSize-1; y++){
    for(int x = 1; x < startSize-1; x++){
      terrainVector.at(y).at(x)->generateGeometry();
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

  // Blend in the new terrain
  for(int x = 0; x < xSize; x++){
    TerrainPatch *p00,*p01,*p10,*p11;
    p01 = terrainVector.at(0).at(x);
    p11 = terrainVector.at(1).at(x);
    blender->blendVert(p01,p11);
      
    if(x != 0){
      p00 = terrainVector.at(0).at(x-1);
      p10 = terrainVector.at(1).at(x-1);
      
      blender->blendHors(p10,p11);
      blender->blendCorners(p00,p01,p10,p11);
    }
  }
  
  for(int x = 1; x < xSize-1; x++){
    terrainVector.at(1).at(x)->generateGeometry();
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

  // Blend in the new terrain
  for(int x = 0; x < xSize; x++){
    TerrainPatch *p00,*p01,*p10,*p11;
    p01 = terrainVector.at(ySize-1).at(x);
    p11 = terrainVector.at(ySize).at(x);
    blender->blendVert(p01,p11);
      
    if(x != 0){
      p00 = terrainVector.at(ySize-1).at(x-1);
      p10 = terrainVector.at(ySize).at(x-1);
      
      blender->blendHors(p10,p11);
      blender->blendCorners(p00,p01,p10,p11);
    }
  }

  for(int x = 1; x < xSize-1; x++){
    terrainVector.at(ySize-1).at(x)->generateGeometry();
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

  // Blend in the new terrain
  for(int y = 0; y < ySize; y++){
    TerrainPatch *p00,*p01,*p10,*p11;
    p10 = terrainVector.at(y).at(0);
    p11 = terrainVector.at(y).at(1);
    blender->blendHors(p10,p11);
      
    if(y != 0){
      p00 = terrainVector.at(y-1).at(0);
      p01 = terrainVector.at(y-1).at(1);
      blender->blendVert(p01,p11);
      blender->blendCorners(p00,p01,p10,p11);
    }
  }
    
  for(int y = 1; y < ySize-1; y++){
    terrainVector.at(y).at(1)->generateGeometry();
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

  // Blend in the new terrain
  for(int y = 0; y < ySize; y++){
    TerrainPatch *p00,*p01,*p10,*p11;
    p10 = terrainVector.at(y).at(xSize-1);
    p11 = terrainVector.at(y).at(xSize);
    blender->blendHors(p10,p11);
      
    if(y != 0){
      p00 = terrainVector.at(y-1).at(xSize-1);
      p01 = terrainVector.at(y-1).at(xSize);
      blender->blendVert(p01,p11);
      blender->blendCorners(p00,p01,p10,p11);
    }
  }
    
  for(int y = 1; y < ySize-1; y++){
    terrainVector.at(y).at(xSize-1)->generateGeometry();
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

  vector<float> heightMapPatch = patchGenerator->generatePatch(patchX, patchY, patchSize);

  return new TerrainPatch(heightMapPatch,patchSize, patchX, patchY,patchOverlap, &terrainShader, &terrainTexture);
}


void threadUpdateTerrain(World *w){
  makeWorkerCurrent();
  w->updateTerrain();
  makeMainContextCurrent();
}


void threadAddPatchNorth(World *w){
  makeWorkerCurrent();
  w->addTerrainNorth();
  w->removeTerrainSouth();
  makeMainContextCurrent();
  w->updatingWorld = false;
}
void threadAddPatchSouth(World *w){
  makeWorkerCurrent();
  w->addTerrainSouth();
  w->removeTerrainNorth();
  makeMainContextCurrent();
  w->updatingWorld = false;
}
void threadAddPatchWest(World *w){
  makeWorkerCurrent();
  w->addTerrainWest();
  w->removeTerrainEast();
  makeMainContextCurrent();
  w->updatingWorld = false;
}
void threadAddPatchEast(World *w){
  makeWorkerCurrent();
  w->addTerrainEast();
  w->removeTerrainWest();
  makeMainContextCurrent();
  w->updatingWorld = false;
}

void World::update(){
  time = (GLfloat)glutGet(GLUT_ELAPSED_TIME)/1000;

  updateTerrain();
  camera->update();

   if(camera->addTerrain != 0){

    if(camera->addTerrain == NORTH){
      camera->addTerrain = 0;
      thread threadNorth(threadAddPatchNorth, this);
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

  for(int y = 0; y < terrainVector.size(); y++){
    for(int x = 0; x < terrainVector.at(y).size(); x++){
      TerrainPatch *patch = terrainVector.at(y).at(x);
      if(camera->isInFrustum(patch)){
  	terrainVector.at(y).at(x)->draw(camera->cameraMatrix);
      }
    }
  }

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
