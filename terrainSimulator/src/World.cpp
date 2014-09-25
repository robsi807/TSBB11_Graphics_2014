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
  skybox = new Skybox(&skyboxShader, camera->projectionMatrix, "../textures/skybox/sky%d.tga");
  terrainPatch = new TerrainPatch(&ttex, 0l ,0l, &phongShader);

}

void World::renderWorld(){
  camera->update();
  mat4 modelView = IdentityMatrix();
  mat4 total = Mult(camera->cameraMatrix,modelView);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  skybox->draw(camera->cameraMatrix);

  glUseProgram(phongShader);
  glUniformMatrix4fv(glGetUniformLocation(phongShader, "mdl2World"), 1, GL_TRUE, modelView.m);
  glUniformMatrix4fv(glGetUniformLocation(phongShader, "world2View"), 1, GL_TRUE, camera->cameraMatrix.m);
  glUniformMatrix4fv(glGetUniformLocation(phongShader, "mdlMatrix"), 1, GL_TRUE, total.m);
  glBindTexture(GL_TEXTURE_2D, terrainPatch->texture);	
  DrawModel(terrainPatch->geometry, phongShader, "inPosition", "inNormal", "inTexCoord"); // "inNormal"

}

World::~World(){
  delete camera;
  delete skybox;
  delete terrainPatch;
}
