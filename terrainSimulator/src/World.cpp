#include "World.h"

World::World(){
  init();
}

void addToVector(World *w, float patchX, float patchY, float patchSize){
	TextureData heightMap = w->patchGenerator->generatePatch(patchSize);

	//printf("heightMap @ %li\n", &heightMap);
	//printf("w-phongShader %li\n", w->phongShader);

	TerrainPatch* terrainPatch = new TerrainPatch(&heightMap, patchX*patchSize , patchY*patchSize, &(w->phongShader),"../textures/grass.tga"); // TODO: dont load the texture for each patch
	//w->terrainVector.push_back(terrainPatch);
}

void World::generatePatch(int patchX, int patchY, float patchSize){

  //TextureData heightMap = patchGenerator->generatePatch(patchSize);

  //TerrainPatch* terrainPatch = new TerrainPatch(&heightMap, patchX*patchSize , patchY*patchSize, &phongShader,"../textures/grass.tga"); // TODO: dont load the texture for each patch

	thread first (addToVector, this, patchX, patchY, patchSize);
	first.join();
  //terrainVector.push_back(terrainPatch);   // took 0.61
}

void World::init(){

  // Load shaders
  phongShader = loadShaders("phong.vert", "phong.frag");
  skyboxShader = loadShaders("skybox.vert", "skybox.frag");

  // Init objects
  
  patchGenerator = new MockupPatchGenerator("../textures/fft-terrain.tga");
  
  camera = new Camera(vec3(24,20,24), 1, 7);
  skybox = new Skybox(&skyboxShader, camera->projectionMatrix, "../textures/skybox/sky%d.tga");

  // Init light
  glUseProgram(phongShader);

  vec3 lightSource = vec3(50.0f, 100.0f, 50.0f);
  GLfloat specularExponent = 50;
  glUniform3fv(glGetUniformLocation(phongShader, "lightSource"), 1, &lightSource.x);
  glUniform1fv(glGetUniformLocation(phongShader, "specularExponent"), 1, &specularExponent);
  
  glUniformMatrix4fv(glGetUniformLocation(phongShader, "projMatrix"), 1, GL_TRUE, camera->projectionMatrix.m);
	
	
	clock_t t;
  t = clock();


  for(int y = 0; y < 3; y++){
    for(int x = 0; x < 3; x++){
      printf("Adding patch @ %i, %i\n", x, y);
			generatePatch(x, y, 255);
    }
  }
  t = clock() - t;
  printf ("Time to generate terrain: %f seconds)\n",((float)t)/CLOCKS_PER_SEC);


}

void World::drawTerrainVector(TerrainPatch* t){
  t->draw(camera->cameraMatrix);
}

void World::draw(){
  camera->update();

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  skybox->draw(camera->cameraMatrix);
  
  //for_each(terrainVector.begin(), terrainVector.end(), drawTerrainVector);
  for(int i = 0; i < terrainVector.size(); i++){
    terrainVector.at(i)->draw(camera->cameraMatrix);
  }   

}


World::~World(){
  delete camera;
  delete skybox;
  terrainVector.clear();
  delete patchGenerator;
}
