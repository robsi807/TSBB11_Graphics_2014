#include "World.h"

World::World(){
	
	time = 0;
	// Load shaders
  init();
}

void World::init(){
  patchOverlap = PATCH_OVERLAP;

  // Load shaders
  phongShader = loadShaders("phong.vert", "phong.frag");
  skyboxShader = loadShaders("skybox.vert", "skybox.frag");

  // Init terrain textures
  // LoadTGATextureSimple("../textures/grass.tga", &terrainTexture);
  //glActiveTexture(GL_TEXTURE0);
  //glUniform1i(glGetUniformLocation(phongShader, "tex"), 0);

  // Init objects
  patchGenerator = new PerlinPatchGenerator();
  
  camera = new Camera(vec3(24,20,24), 1, 7);
  skybox = new Skybox(&skyboxShader, camera->projectionMatrix, "../textures/skybox/skybox2/sky%d.tga");
  blender = new LinearBlender(patchOverlap);

  // Init light
  glUseProgram(phongShader);

  vec3 lightSource = vec3(50.0f, 100.0f, 50.0f);
  GLfloat specularExponent = 50;
  glUniform3fv(glGetUniformLocation(phongShader, "lightSource"), 1, &lightSource.x);
  glUniform1fv(glGetUniformLocation(phongShader, "specularExponent"), 1, &specularExponent);

  glUniformMatrix4fv(glGetUniformLocation(phongShader, "projMatrix"), 1, GL_TRUE, camera->projectionMatrix.m);

  // Initiate height maps
  for(int y = 0; y < 2; y++){
    for(int x = 0; x < 2; x++){
      printf("Adding patch @ %i, %i\n", x, y);
      generatePatch(x, y, 256);
    }
  }

  // Blend height maps
  // Such hard code, very re-do prez. 
  TerrainPatch *p00,*p01,*p10,*p11;
  p00 = terrainVector.at(0*2 + 0);
  p01 = terrainVector.at(0*2 + 1);
  p10 = terrainVector.at(1*2 + 0);
  p11 = terrainVector.at(1*2 + 1);
  blender->blendCorners(p00,p01,p10,p11);
  blender->blendHors(p00,p01);
  blender->blendHors(p10,p11);
  blender->blendVert(p00,p10);
  blender->blendVert(p01,p11);

  // Generate geometry
  for(int y = 0; y < 2; y++){
    for(int x = 0; x < 2; x++){
      terrainVector.at(y*2 + x)->generateGeometry();
    }
  }
}

void World::generatePatch(int patchX, int patchY, int patchSize){

  //printf("before generatePatch\n");
  vector<float> heightMapPatch = patchGenerator->generatePatch(patchX, patchY, patchSize);
  //printf("after generatePatch, before terrainPatch\n");

  //terrainPatch = (TerrainPatch*)malloc(sizeof(TerrainPatch));
  //memset(terrainPatch, 0, sizeof(TerrainPatch));

    TerrainPatch* terrainPatch = new TerrainPatch(heightMapPatch,patchSize, patchSize, patchX*(patchSize-PATCH_OVERLAP) , patchY*(patchSize-PATCH_OVERLAP),PATCH_OVERLAP, &phongShader, &terrainTexture); // TODO: dont load the texture for each patch

    //  TerrainPatch* terrainPatch = new TerrainPatch(heightMapPatch,patchSize, patchSize, patchX*patchSize , patchY*patchSize, &phongShader,"../textures/grass.tga"); // TODO: dont load the texture for each patch
  terrainVector.push_back(terrainPatch);
  //printf("after terrainPatch\n");

}

void World::update(){
  time = (GLfloat)glutGet(GLUT_ELAPSED_TIME)/1000;
  updateTerrain(camera->getPosition(), camera->getDirection());
  camera->update();
  //addGeneratedTerrain();

}

void World::draw(){

  update();

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  skybox->draw(camera->cameraMatrix);

  for(int i = 0; i < terrainVector.size(); i++){
    terrainVector.at(i)->draw(camera->cameraMatrix);

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

//------------ ABOVE used by threding, broken code :( ----------------

