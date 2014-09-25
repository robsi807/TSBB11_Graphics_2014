#include "World.h"

World::World(){
  initWorld();
}

void World::initWorld(){

  phongShader = loadShaders("phong.vert", "phong.frag");
  skyboxShader = loadShaders("skybox.vert", "skybox.frag");

  TextureData heightMap;
  LoadTGATextureData("../textures/fft-terrain.tga", &heightMap);


  camera = new Camera(vec3(24,20,24), 0.7, 7);
  skybox = new Skybox(&skyboxShader, camera->projectionMatrix, "../textures/skybox/sky%d.tga");
  terrainPatch = new TerrainPatch(&heightMap, 0.0 ,0.0, &phongShader,"../textures/grass.tga");
  terrainPatch2 = new TerrainPatch(&heightMap,heightMap.width ,0.0, &phongShader,"../textures/grass.tga");

}

void World::renderWorld(){
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
