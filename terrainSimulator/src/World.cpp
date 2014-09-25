#include "World.h"

World::World(){
  initWorld();
}

void World::initWorld(){

  phongShader = loadShaders("phong.vert", "phong.frag");
  skyboxShader = loadShaders("skybox.vert", "skybox.frag");

  TextureData ttex; // terrain
  LoadTGATextureData("../textures/fft-terrain.tga", &ttex);


  camera = new Camera(vec3(24,20,24), 0.7, 7);
  skybox = new Skybox(&skyboxShader);
  terrainPatch = new TerrainPatch(&ttex, 0l ,0l);

}

World::~World(){
  delete camera;
  delete skybox;
  delete terrainPatch;
}
