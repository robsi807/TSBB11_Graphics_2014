#include "World.h"

World::World(){
  init();
}

void printTerrain(TerrainPatch* t){
  cout << "Patch @(" << t->posX << ", " << t->posY << ") has terrain data:\n";
  for(int i = 0; i < 10; i++){
    cout << &(t->geometry->vertexArray[i]) << "with value " << t->geometry->vertexArray[i] <<"\n";
  }
  cout << "The patch is allocated at " << t << "\n";
  cout << "----------\n";
}

void addToVector(World *w, float patchX, float patchY, float patchSize, Model *terrain, TerrainPatch *terrainPatch){

  TextureData heightMap = w->patchGenerator->generatePatch(patchSize);
  terrainPatch = new TerrainPatch(&heightMap, patchX*patchSize , patchY*patchSize, &(w->phongShader),"../textures/grass.tga", terrain); // TODO: dont load the texture for each patch
  w->generatedTerrain.push_back(terrainPatch);
  
  printTerrain(w->generatedTerrain.back());

}

void World::generatePatch(int patchX, int patchY, float patchSize){

  Model *terrain = new Model();
  //  memset(test, 0, sizeof(Model));
  TerrainPatch *terrainPatch;

  //thread first (addToVector, this, patchX, patchY, patchSize, terrain, terrainPatch);
  //first.detach();
  //first.join();
  addToVector(this, patchX, patchY, patchSize, terrain, terrainPatch);


}


void World::addGeneratedTerrain(){

  if(generatedTerrain.size() > 0){
    clock_t t;
    t = clock();
    for(int i = 0; i < generatedTerrain.size(); i++){
      BuildModelVAO2(generatedTerrain.at(i)->geometry);
      terrainVector.push_back(generatedTerrain.at(i));	
      
      printTerrain(generatedTerrain.back());
    }
    t = clock() - t;
    printf ("Time to generate terrain: %f seconds)\n",((float)t)/CLOCKS_PER_SEC);

    generatedTerrain.clear();
  }
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

  generatePatch(1, 2, 255);



  for(int y = 0; y < 3; y++){
    for(int x = 0; x < 3; x++){
      //generatePatch(x, y, 255);
    }
  }



}

void World::drawTerrainVector(TerrainPatch* t){
  t->draw(camera->cameraMatrix);
}

void World::draw(){

  addGeneratedTerrain();
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
