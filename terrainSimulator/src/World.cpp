#include "World.h"

World::World(){
	
	time = 0;
	// Load shaders
  phongShader = loadShaders("phong.vert", "phong.frag");
  skyboxShader = loadShaders("skybox.vert", "skybox.frag");

  // Init objects
  
  patchGenerator = new PerlinPatchGenerator();
  
  camera = new Camera(vec3(24,20,24), 1, 7);
  skybox = new Skybox(&skyboxShader, camera->projectionMatrix, "../textures/skybox/skybox2/sky%d.tga");
  blender = new LinearBlender(PATCH_OVERLAP);

  // Init light
  glUseProgram(phongShader);

  vec3 lightSource = vec3(50.0f, 100.0f, 50.0f);
  GLfloat specularExponent = 50;
  glUniform3fv(glGetUniformLocation(phongShader, "lightSource"), 1, &lightSource.x);
  glUniform1fv(glGetUniformLocation(phongShader, "specularExponent"), 1, &specularExponent);

  glUniformMatrix4fv(glGetUniformLocation(phongShader, "projMatrix"), 1, GL_TRUE, camera->projectionMatrix.m);

  for(int y = 0; y < 3; y++){
    for(int x = 0; x < 3; x++){
      generatePatch(x, y, 256);
    }
  }

}

void World::generatePatch(int patchX, int patchY, int patchSize){

  printf("before generatePatch");
  vector<float> heightMapPatch = patchGenerator->generatePatch(patchX, patchY, patchSize);
  printf("after generatePatch, before terrainPatch");

  TerrainPatch* terrainPatch = new TerrainPatch(heightMapPatch,patchSize, patchSize, patchX*patchSize , patchY*patchSize, &phongShader,"../textures/grass.tga"); // TODO: dont load the texture for each patch
  terrainVector.push_back(terrainPatch);
  printf("after terrainPatch");

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
  terrainVector.clear();
  delete patchGenerator;
}


// -------  Was used by threading, not working after merge -------

void addToVector(World *w, int patchX, int patchY, int patchSize, Model *terrain){

  vector<float> heightMap = w->patchGenerator->generatePatch(patchX, patchY, patchSize);
  TerrainPatch* terrainPatch = new TerrainPatch(heightMap, patchX*patchSize , patchY*patchSize, patchSize, patchSize, &(w->phongShader),"../textures/grass.tga"); // TODO: dont load the texture for each patch
  w->generatedTerrain.push_back(terrainPatch);
  
}

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

