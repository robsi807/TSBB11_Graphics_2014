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
  patchGenerator = new ValuePatchGenerator();

  camera = new Camera(vec3(0,60,0), 1, 7);
  skybox = new Skybox(&skyboxShader, camera->projectionMatrix, "../textures/skybox/skybox2/sky%d.tga");
  blender = new LinearBlender(patchOverlap);

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
  
  testGenerateStartingPatches(GRID_BEGIN_SIZE);
}

void World::testGenerateStartingPatches(int startSize) {

  // Initiate height maps
  cout << "generating test patches... \n";
  for(int y = -(startSize-1)/2; y <= (startSize-1)/2; y++){
    vector<vec3> testRow;
    for(int x = -(startSize-1)/2; x <= (startSize-1)/2; x++){
      printf("Adding test ''patch'' @ %i, %i\n", x, y);
      testRow.push_back(vec3(x,y,0)); // hard coded for test case...
    }
    testVector.push_back(testRow);
  }
  
  
  cout << "DONE generating test patches... \n";
  
  printTestVector();
  

}

void World::printTestVector() {

  int size = testVector.size();
    
  vec3 value;
  int xValue = 0;
  int yValue = 0;
    
  for(int y = 0;y < size; y++){
    for(int x = 0; x < size; x++){
      value = testVector.at(y).at(x);
      
      //xValue = value % 10;
      //yValue = (value - xValue) / 10;
      
      //cout << "(" << xValue << "," << yValue << ") " << value << " ";
      
      cout << "(" << value.x << "," << value.y << ") ";
      
    }
    cout << "\n";
  }
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


void World::testAddPatchRow(int direction) {

}



void World::addPatchRowNew(int direction) {

  int xSize = terrainVector.at(0).size();
  int ySize = terrainVector.size();
  vector<TerrainPatch*> newTerrainVec;

  printf("Terrain grid size: (%i,%i)\n",xSize,ySize);

  int yOffSet = 0;
  int xOffSet = 0;

  if(direction == NORTH){
    yOffSet = 1;
  } else if(direction == SOUTH){
    yOffSet = -1;
  } else if(direction == EAST){
    xOffSet = 1;
  } else if(direction == WEST){
    xOffSet = -1;
  }
  
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

  /* first we construct a small test case!

  // direction not needed (if we move backwards for example, it shouldn't affect where we remove and add patches).
  
  // check if position is in new patch.
  int middlePatchIndex = 0 + (terrainVector.size() - 1)/2; // get the middle patch, zero indexed
  
  float cameraX = camera->getPosition().x;
  float cameraY = camera->getPosition().z;
  
  float terrainX = terrainVector.at(middlePatchIndex).at(middlePatchIndex)->xPos;
  float terrainY = terrainVector.at(middlePatchIndex).at(middlePatchIndex)->yPos;
  float terrainSize = terrainVector.at(middlePatchIndex).at(middlePatchIndex)->size;
  
  //printf("updating terrain... %d is the middle index! \n" , middlePatchIndex);
  //printf("current pos is: %d, %d \n", position.x, position.z);
  //printf("current patch position is: %d, %d \n", terrainX, terrainY);
  //cout << "current pos is: " << camera->getPosition().x << " " << camera->getPosition().z << "\n";
   
  //cout << "middle patch position is is: " << terrainX << " " << terrainY << "\n";
  
  if(cameraX < terrainX) {
    //cout << "We will shortly update Western border \n";
  
  } else if (cameraX > terrainX + terrainSize) {
    //cout << "We will shortly update Eastern border \n";
  } // else do nothing.
  
  if(cameraY < terrainY) {
    cout << "We will shortly update Southern border \n";
    addPatchRow(SOUTH);
  } else if (cameraY > terrainY + terrainSize) {
    //cout << "We will shortly update Northern border \n";
  } // else do nothing.
  
  
  */
    
  float TEST_SIZE = 50.0;
    
  float cameraX = camera->getPosition().x;
  float cameraY = camera->getPosition().z;
  
  int middleIndex = (testVector.size()-1)/2 + 0;
  
  vec3 gridPos = testVector.at(middleIndex).at(middleIndex);
    
  cout << "current position is: (" << cameraX << "," << cameraY << ") \n";
  
  cout << "\n\n\n\n\n\n\n\n\n\n";
  
  printTestVector();
  
  
  
  //cout << "current position is: (" << cameraX << "," << cameraY << ") \n";
  
  cout << "current grid position is: (" << gridPos.x*TEST_SIZE << "," << gridPos.y*TEST_SIZE << ") \n";
  
  if(cameraX < gridPos.x * TEST_SIZE) {
    cout << "We will shortly update Western border \n";
    moveTestWorld(WEST);
  
  } else if (cameraX > gridPos.x*TEST_SIZE + TEST_SIZE) {
    cout << "We will shortly update Eastern border \n";
    moveTestWorld(EAST);
  } // else do nothing.
  
  if(cameraY < gridPos.y*TEST_SIZE) {
    cout << "We will shortly update Southern border \n";
    moveTestWorld(SOUTH);
  } else if (cameraY > gridPos.y*TEST_SIZE + TEST_SIZE) {
    cout << "We will shortly update Northern border \n";
    
    moveTestWorld(NORTH);
  } 
  
  //terrainVector
  
}


void World::moveTestWorld(int direction) {

  
  if(direction == SOUTH || direction == NORTH) {
    moveTestWorldHor(direction);
  } else {
    moveTestWorldVer(direction);
  }
}

void World::moveTestWorldVer(int direction) {
  int originalSize = testVector.size();
  
  if(direction == WEST) {
  
    int dirOffSet = -1;
  
  
    std::vector< std::vector<vec3> >::iterator row; 
    std::vector<vec3>::iterator col; 

    cout << "printing vector troll \n";

    for (row = testVector.begin(); row != testVector.end(); ++row)
    { 
        cout << "Attempting to access element " << std::distance(testVector.begin(), row) << " of testVector \n";
        //vector<vec3> testRow = *row; 
        cout << "Erasaing first elem in row...\n";
        row->erase(row->begin());
        
        cout << "accessing element nbr " << std::distance(row->begin(), row->end()-1)<< "...\n";
        vec3 prevElem = row->at(std::distance(row->begin(), row->end()-1));
        
        row->push_back(vec3(prevElem.x+dirOffSet, prevElem.y, prevElem.z));
    } 
    
    cout << "DONE printing vector troll \n";
  
  } else {
    
    int dirOffSet = 1;
  
  
    std::vector< std::vector<vec3> >::iterator row; 
    std::vector<vec3>::iterator col; 

    cout << "printing vector troll \n";

    for (row = testVector.begin(); row != testVector.end(); ++row)
    { 
        cout << "Attempting to access element " << std::distance(testVector.begin(), row) << " of testVector \n";
        //vector<vec3> testRow = *row; 
        cout << "Erasaing first elem in row...\n";
        row->erase(row->end());
        
        cout << "accessing element nbr " << std::distance(row->begin(), row->begin())<< "...\n";
        vec3 prevElem = row->at(std::distance(row->begin(), row->begin()));
        
        row->insert(row->begin(),vec3(prevElem.x+dirOffSet, prevElem.y, prevElem.z));
    } 
    
    cout << "DONE printing vector troll \n";
  
  
  }
  

}


void World::moveTestWorldHor(int direction) {
    // will turn into subfunction moveTestWorldHor...
    
    int originalSize = testVector.size();
    
    std::vector<vector<vec3>>::iterator eraseRowIter;
    std::vector<vector<vec3>>::iterator prevRowIter;
    int dirOffSet;

    if(direction == SOUTH) {
      eraseRowIter = testVector.begin();
      prevRowIter = testVector.end() - 2; // -2 to compensate for erased vector.
      dirOffSet = -1;
    } else if (direction == NORTH) {
    
      eraseRowIter = testVector.end();
      prevRowIter = testVector.begin(); // -2 to compensate for erased vector.
      dirOffSet = +1;
    
    }
  
    testVector.erase(eraseRowIter);
    
    // construct and add another row
    cout << "Trying the buggy one" << std::distance(testVector.begin(), prevRowIter) << "\n";
    vector<vec3> prevRow = testVector.at(std::distance(testVector.begin(), prevRowIter));
    
    cout << "Tried the buggy one\n";
    vector<vec3> testRow;
    vec3 prevElement;
    for (int i = 0; i < originalSize;i++) {
      prevElement = prevRow.at(i);
      
    // if normal mode, we add patch
      testRow.push_back(vec3(prevElement.x, prevElement.y+dirOffSet, prevElement.z));
    }
    
    if(direction == SOUTH) {
      testVector.push_back(testRow); // add to end if South.
    } else {
      testVector.insert(testVector.begin(), testRow); // add to start if North.
    }
    
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

