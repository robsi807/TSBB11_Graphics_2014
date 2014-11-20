#include "World.h"

World::World(){
	
  time = 0;

  patchOverlap = PATCH_OVERLAP;
  patchSize = PATCH_SIZE;
  gridSize = GRID_BEGIN_SIZE;

  // Load shaders
  terrainShader = loadShaders("shaders/terrain.vert","shaders/terrain.frag");
  //terrainShader = loadShadersG("shaders/grass.vert","shaders/grass.frag","shaders/passthrough.gs");
  //  phongShader = loadShaders("shaders/phong.vert", "shaders/phong.frag");
  skyboxShader = loadShaders("shaders/skybox.vert", "shaders/skybox.frag");
  grassShader = loadShadersG("shaders/grass.vert","shaders/grass.frag","shaders/grass.gs");

  // Init objects
  //patchGenerator = new PerlinPatchGenerator();
  patchGenerator = new ValuePatchGenerator();

  camera = new Camera(vec3(0,60,0), 1, 7);
  skybox = new Skybox(&skyboxShader, camera->projectionMatrix, "../textures/skybox/skybox2/sky%d.tga");
  blender = new LinearBlender(patchOverlap);

  // Init light to shaders
  glUseProgram(terrainShader);

  vec3 lightDir = Normalize(vec3(0.0f, 2.0f, 1.0f));
  GLfloat specularExponent = 2.0;
  glUniform3fv(glGetUniformLocation(terrainShader, "lightDirection"), 1, &lightDir.x);
  glUniform1fv(glGetUniformLocation(terrainShader, "specularExponent"), 1, &specularExponent);
  glUniformMatrix4fv(glGetUniformLocation(terrainShader, "projMatrix"), 1, GL_TRUE, camera->projectionMatrix.m);
 
  glUseProgram(grassShader);
  glUniform3fv(glGetUniformLocation(grassShader, "lightDirection"), 1, &lightDir.x);
  glUniform1fv(glGetUniformLocation(grassShader, "specularExponent"), 1, &specularExponent);
  glUniformMatrix4fv(glGetUniformLocation(grassShader, "projMatrix"), 1, GL_TRUE, camera->projectionMatrix.m);  
  
  // Upload textures to terrain shader
  glUseProgram(terrainShader);
  GLuint grassTex1;
  glActiveTexture(GL_TEXTURE0);
  LoadTGATextureSimple("../textures/grass4_1024.tga", &grassTex1);
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

  glUseProgram(grassShader);
  // Upload textures to grass shader
  GLuint grassTexLowPass;
  glActiveTexture(GL_TEXTURE0+4);
  LoadTGATextureSimple("../textures/grass4_1024_lp.tga",&grassTexLowPass);
  glUniform1i(glGetUniformLocation(grassShader,"grassTex"),4);

  GLuint noiseTex2;
  glActiveTexture(GL_TEXTURE0+5);
  LoadTGATextureSimple("../textures/noise/uniformNoise1.tga",&noiseTex2);
  glUniform1i(glGetUniformLocation(grassShader,"noiseTex"),4);

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

// Adds terrain patches in the direction specified
void World::addPatchRow(int direction){
  int xSize = terrainVector.at(0).size();
  int ySize = terrainVector.size();
  vector<TerrainPatch*> newTerrainVec;

  printf("Terrain grid size: (%i,%i)\n",xSize,ySize);

  if(direction == NORTH){
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
  else if(direction == SOUTH){
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
  else if(direction == WEST){
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
    printf("Terrain added west at xGrid = %i\n",xGrid);
  }
  else if(direction == EAST){
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
}

TerrainPatch* World::generatePatch(int patchX, int patchY){

  vector<float> heightMapPatch = patchGenerator->generatePatch(patchX, patchY, patchSize);

  return new TerrainPatch(heightMapPatch,patchSize, patchX, patchY,patchOverlap, &terrainShader, &grassShader);
}

void threadAddPatchRow(World *w, int dir){
  makeWorkerCurrent();
  w->addPatchRow(dir);
  makeMainContextCurrent();
}

void World::update(){
  time = (GLfloat)glutGet(GLUT_ELAPSED_TIME)/1000;
  updateTerrain(camera->getPosition(), camera->getDirection());
  camera->update();
 
  // DEBUGGING PURPOSE CODE START
  if(camera->addTerrain != 0){

    if(camera->addTerrain == NORTH){
      camera->addTerrain = 0;
      //thread threadNorth(threadAddPatchRow, this, NORTH);
      //threadNorth.detach();
      addPatchRow(NORTH);
    }
    if(camera->addTerrain == SOUTH){
      camera->addTerrain = 0;
      //thread threadSouth(threadAddPatchRow, this, SOUTH);
      //threadSouth.detach();
      addPatchRow(SOUTH);
    }
    if(camera->addTerrain == EAST){
      camera->addTerrain = 0;
      //thread threadEast(threadAddPatchRow, this, EAST);
      //threadEast.detach();
      addPatchRow(EAST);
    }
    if(camera->addTerrain == WEST){
      camera->addTerrain = 0;
      //thread threadWest(threadAddPatchRow, this, WEST);
      //threadWest.detach();
      addPatchRow(WEST);
    }
  }
  // DEBUGGING PURPOSE CODE END
}

void World::draw(){

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  skybox->draw(camera->cameraMatrix);

  for(int y = 0; y < terrainVector.size(); y++){
    for(int x = 0; x < terrainVector.at(y).size(); x++){
      TerrainPatch *patch = terrainVector.at(y).at(x);
      if(camera->isInFrustum(patch)){
	patch->draw(camera->cameraMatrix,time);

      }
    }
  }
}


void World::updateTerrain(vec3 position, vec3 direction){

}


World::~World(){
  delete camera;
  delete skybox;
  delete blender;
  terrainVector.clear();
  delete patchGenerator;
}

