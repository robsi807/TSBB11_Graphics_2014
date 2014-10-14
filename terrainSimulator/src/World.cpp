#include "World.h"

World::World(){
  init();
}

void World::init(){
  patchOverlap = PATCH_OVERLAP;

  // Load shaders
  phongShader = loadShaders("phong.vert", "phong.frag");
  skyboxShader = loadShaders("skybox.vert", "skybox.frag");

  // Init objects
  patchGenerator = new PerlinPatchGenerator();
  
  camera = new Camera(vec3(24,20,24), 1, 7);
  skybox = new Skybox(&skyboxShader, camera->projectionMatrix, "../textures/skybox/sky%d.tga");
  blender = new LinearBlender(patchOverlap);

  // Init light
  glUseProgram(phongShader);

  vec3 lightSource = vec3(50.0f, 100.0f, 50.0f);
  GLfloat specularExponent = 50;
  glUniform3fv(glGetUniformLocation(phongShader, "lightSource"), 1, &lightSource.x);
  glUniform1fv(glGetUniformLocation(phongShader, "specularExponent"), 1, &specularExponent);
  
  glUniformMatrix4fv(glGetUniformLocation(phongShader, "projMatrix"), 1, GL_TRUE, camera->projectionMatrix.m);

  // Initiate height maps
  int x, y;
  for(y = 0; y < 2; y++){
    for(x = 0; x < 2; x++){
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
  for(y = 0; y < 2; y++){
    for(x = 0; x < 2; x++){
      terrainVector.at(y*2 + x)->generateGeometry();
    }
  }
}


void World::generatePatch(int patchX, int patchY, int patchSize){

    vector<float> heightMapPatch = patchGenerator->generatePatch(patchX, patchY, patchSize);
    
    //patchGenerator->printMatrix(heightMapPatch, patchSize);

    TerrainPatch* terrainPatch = new TerrainPatch(heightMapPatch,patchSize, patchSize, patchX*(patchSize-PATCH_OVERLAP) , patchY*(patchSize-PATCH_OVERLAP),PATCH_OVERLAP, &phongShader,"../textures/grass.tga"); // TODO: dont load the texture for each patch

  cout << "Init mem: " << &terrainPatch->blendedHeightMap << endl;
  cout << "Init mem 2: " << &heightMapPatch << endl;

  terrainVector.push_back(terrainPatch);
}


void World::drawTerrainVector(TerrainPatch* t){
  t->draw(camera->cameraMatrix);
}

void World::draw(){
  camera->update();

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  skybox->draw(camera->cameraMatrix);
  
  //for_each(terrainVector.begin(), terrainVector.end(), drawTerrainVector);
  //printf("size of vector = %i\n", terrainVector.size());
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
