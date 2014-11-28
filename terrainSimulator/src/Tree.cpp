#include "Tree.h"

// Static variables
Model* Tree::model;
Model* Tree::trunkModel;
GLuint* Tree::shader;
GLuint* Tree::geomShader;
GLuint Tree::woodTexture;

// Initialize static variables
void Tree::initTrees(GLuint *shade,GLuint *geoShade){
  shader = shade;
  geomShader = geoShade;   
  model = LoadModelPlus("../objects/tree2.obj");
  trunkModel = LoadModelPlus("../objects/tree2trunk.obj");
  glActiveTexture(GL_TEXTURE0+7);
  LoadTGATextureSimple("../textures/tree1_1024.tga",&woodTexture);
  glUniform1i(glGetUniformLocation(*shader,"tex"),7);
}   

Tree::Tree(vec3 pos,float yRot,float scaling,vec3 terrainPos){  
  position = pos;
  globalPosition = pos + terrainPos;
  scale = scaling;
  mat4 transTerrain = T(terrainPos.x,0.0,terrainPos.z);  
  mat4 trans = T(position.x,position.y,position.z);
  mat4 rot = Ry(yRot);
  mat4 scaleMat = S(scaling,scaling,scaling);
  mdl2World =  transTerrain * trans * rot * scaleMat;
}


void Tree::draw(mat4 cameraMatrix,float time){
  
  glUseProgram(*shader);
  float specularExponent = 2.0;
  glUniform1fv(glGetUniformLocation(*shader, "specularExponent"), 1, &specularExponent);
  glUniformMatrix4fv(glGetUniformLocation(*shader, "mdl2World"), 1, GL_TRUE, mdl2World.m);
  glUniformMatrix4fv(glGetUniformLocation(*shader, "world2View"), 1, GL_TRUE, cameraMatrix.m);
  glBindTexture(GL_TEXTURE_2D,woodTexture);
  // Draw branches
  DrawModel(model, *shader, "inPosition", "inNormal","inTexCoord"); 
  // Draw trunk
  DrawModel(trunkModel,*shader,"inPosition", "inNormal","inTexCoord");

  // Draw Leaves
  glEnable (GL_POLYGON_SMOOTH);
  glUseProgram(*geomShader);
  glUniformMatrix4fv(glGetUniformLocation(*geomShader, "mdl2World"), 1, GL_TRUE, mdl2World.m);
  glUniformMatrix4fv(glGetUniformLocation(*geomShader, "world2View"), 1, GL_TRUE, cameraMatrix.m);
  glUniform1f(glGetUniformLocation(*geomShader,"time"), time); 
  DrawModel(model, *geomShader, "inPosition", "inNormal","inTexCoord");
  glDisable (GL_POLYGON_SMOOTH);
  
}
