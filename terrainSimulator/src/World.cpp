#include "World.h"

World::World(){
	
  time = 0;

  patchOverlap = PATCH_OVERLAP;
  patchSize = PATCH_SIZE;
  gridSize = GRID_BEGIN_SIZE;

  // Load shaders
  terrainShader = loadShaders("shaders/terrain.vert","shaders/terrain.frag");
  phongShader = loadShaders("shaders/phong.vert", "shaders/phong.frag");
  skyboxShader = loadShaders("shaders/skybox.vert", "shaders/skybox.frag");

  // Init terrain textures
  // LoadTGATextureSimple("../textures/grass.tga", &terrainTexture);
  //glActiveTexture(GL_TEXTURE0);
  //glUniform1i(glGetUniformLocation(phongShader, "tex"), 0);

  // Init objects
  patchGenerator = new PerlinPatchGenerator();
  //patchGenerator = new ValuePatchGenerator();

  camera = new Camera(vec3(50,120,50), 1, 7);
  skybox = new Skybox(&skyboxShader, camera->projectionMatrix, "../textures/skybox/skybox2/sky%d.tga");
  blender = new LinearBlender(patchOverlap);

  // Init light to terrain shader
  glUseProgram(terrainShader);

  vec3 lightSource = vec3(50.0f, 100.0f, 50.0f);
  GLfloat specularExponent = 20.0;
  glUniform3fv(glGetUniformLocation(terrainShader, "lightSource"), 1, &lightSource.x);
  glUniform1fv(glGetUniformLocation(terrainShader, "specularExponent"), 1, &specularExponent);

  glUniformMatrix4fv(glGetUniformLocation(terrainShader, "projMatrix"), 1, GL_TRUE, camera->projectionMatrix.m);
  
  generateStartingPatches(GRID_BEGIN_SIZE);
}

void World::generateStartingPatches(int startSize){


  // Initiate height maps
  for(int y = 0; y < startSize; y++){
    vector<TerrainPatch*> terrainRow;
    for(int x = 0; x < startSize; x++){
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
      printf("Generating patch @ %i, %i\n", x, y);
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
    // TODO: Geometry generation should be moved from here
    for(int x = 1; x < xSize-1; x++){
      terrainVector.at(ySize-1).at(x) -> generateGeometry();
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
    // TODO: Geometry generation should be moved from here
    for(int x = 1; x < xSize-1; x++){
      terrainVector.at(1).at(x) -> generateGeometry();
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
    
    // TODO: Geometry generation should be moved from here
    for(int y = 1; y < ySize-1; y++){
      terrainVector.at(y).at(1) -> generateGeometry();
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
    
    // TODO: Geometry generation should be moved from here
    for(int y = 1; y < ySize-1; y++){
      terrainVector.at(y).at(xSize-1) -> generateGeometry();
    }
    printf("Terrain added east at xGrid = %i\n",xGrid);
    
  }
}

TerrainPatch* World::generatePatch(int patchX, int patchY){

  //printf("before generatePatch\n");
  vector<float> heightMapPatch = patchGenerator->generatePatch(patchX, patchY, patchSize);
  //printf("after generatePatch, before terrainPatch\n");

  //terrainPatch = (TerrainPatch*)malloc(sizeof(TerrainPatch));
  //memset(terrainPatch, 0, sizeof(TerrainPatch));

    return new TerrainPatch(heightMapPatch,patchSize, patchX, patchY,patchOverlap, &terrainShader, &terrainTexture); // TODO: dont load the texture for each patch

    //  TerrainPatch* terrainPatch = new TerrainPatch(heightMapPatch,patchSize, patchSize, patchX*patchSize , patchY*patchSize, &phongShader,"../textures/grass.tga"); // TODO: dont load the texture for each patch
  //printf("after terrainPatch\n");
}

void World::update(){
  time = (GLfloat)glutGet(GLUT_ELAPSED_TIME)/1000;
  updateTerrain(camera->getPosition(), camera->getDirection());
  camera->update();
  //addGeneratedTerrain();

  // DEBUGGING PURPOSE CODE START
  if(camera->addTerrain != 0){
    if(camera->addTerrain == NORTH){
      camera->addTerrain = 0;
      addPatchRow(NORTH);
    } 
    if(camera->addTerrain == SOUTH){
      camera->addTerrain = 0;
      addPatchRow(SOUTH);
    }
    if(camera->addTerrain == EAST){
      camera->addTerrain = 0;
      addPatchRow(EAST);
    }
    if(camera->addTerrain == WEST){
      camera->addTerrain = 0;
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
      if(camera->isInFrustum(patch))
	patch->draw(camera->cameraMatrix);
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


// -------  Was used by threading, not working after merge -------

// void addToVector(World *w, int patchX, int patchY, int patchSize, Model *terrain){

//   vector<float> heightMap = w->patchGenerator->generatePatch(patchX, patchY, patchSize);
//   TerrainPatch* terrainPatch = new TerrainPatch(heightMap, patchX*patchSize , patchY*patchSize, patchSize, patchSize, &(w->phongShader),"../textures/grass.tga"); // TODO: dont load the texture for each patch
//   w->generatedTerrain.push_back(terrainPatch);
  
// }
/*
void World::addGeneratedTerrain(){

  if(generatedTerrain.size() > 0){
    clock_t t;
    t = clock();
    for(int i = 0; i < generatedTerrain.size(); i++){
      BuildModelVAO2(generatedTerrain.at(i)->geometry);
      terrainVector.push_back(generatedTerrain.at(i));	
    }
    t = clock() - t;
    printf ("Time to generate terrain: %f seconds)\n",((float)t)/CLOCKS_PER_SEC);

    generatedTerrain.clear();
  }
}
*/
//------------ ABOVE used by threding, broken code :( ----------------

