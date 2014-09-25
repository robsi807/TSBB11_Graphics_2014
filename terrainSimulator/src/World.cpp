#include "World.h"

World::World(){
  init();
}

void World::init(){

  // Load shaders
  phongShader = loadShaders("phong.vert", "phong.frag");
  skyboxShader = loadShaders("skybox.vert", "skybox.frag");

  // Load height map
  TextureData heightMap;
  LoadTGATextureData("../textures/fft-terrain.tga", &heightMap);

  // Init objects
  camera = new Camera(vec3(24,20,24), 1, 7);
  skybox = new Skybox(&skyboxShader, camera->projectionMatrix, "../textures/skybox/sky%d.tga");
  terrainPatch = new TerrainPatch(&heightMap, 0.0 ,0.0, &phongShader,"../textures/grass.tga");
  terrainPatch2 = new TerrainPatch(&heightMap,heightMap.width-1,0.0, &phongShader,"../textures/grass.tga");

  // Init light
  vec3 lightSource = vec3(50.0f, 100.0f, 50.0f);
  GLfloat specularExponent = 50;
  
  glUseProgram(phongShader);
  glUniformMatrix4fv(glGetUniformLocation(phongShader, "projMatrix"), 1, GL_TRUE, camera->projectionMatrix.m);
  glUniform3fv(glGetUniformLocation(phongShader, "lightSource"), 1, &lightSource.x);
  glUniform1fv(glGetUniformLocation(phongShader, "specularExponent"), 1, &specularExponent);

}

void World::draw(){
  camera->update();

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  skybox->draw(camera->cameraMatrix);
  terrainPatch->draw(camera->cameraMatrix);
  terrainPatch2->draw(camera->cameraMatrix);

}

World::~World(){
  delete camera;
  delete skybox;
  delete terrainPatch;
}
