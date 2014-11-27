#include "Plant.h"

// Static variables
Model* Plant::model;
GLuint* Plant::shader;
GLuint* Plant::geomShader;
GLuint Plant::woodTexture;

// Initialize static variables
void Plant::initPlants(GLuint *shade,GLuint *geoShade){
   shader = shade;
   geomShader = geoShade;   
   model = LoadModelPlus("../objects/tree1.obj");
  glActiveTexture(GL_TEXTURE0+7);
  LoadTGATextureSimple("../textures/tree1_1024.tga",&woodTexture);
  glUniform1i(glGetUniformLocation(*shader,"tex"),7);
}   

Plant::Plant(vec3 pos,float yRot,float scaling,vec3 terrainPos){  
  position = pos;
  globalPosition = pos + terrainPos;
  scale = scaling;
  mat4 transTerrain = T(terrainPos.x,0.0,terrainPos.z);  
  mat4 trans = T(position.x,position.y,position.z);
  mat4 rot = Ry(yRot);
  mat4 scaleMat = S(scaling,scaling,scaling);
  mdl2World =  transTerrain * trans * rot * scaleMat;
}


void Plant::draw(mat4 cameraMatrix,float time){
  
  glUseProgram(*shader);
  float specularExponent = 2.0;
  glUniform1fv(glGetUniformLocation(*shader, "specularExponent"), 1, &specularExponent);
  glUniformMatrix4fv(glGetUniformLocation(*shader, "mdl2World"), 1, GL_TRUE, mdl2World.m);
  glUniformMatrix4fv(glGetUniformLocation(*shader, "world2View"), 1, GL_TRUE, cameraMatrix.m);
  glBindTexture(GL_TEXTURE_2D,woodTexture);
  DrawModel(model, *shader, "inPosition", "inNormal","inTexCoord"); 

  glEnable (GL_POLYGON_SMOOTH);
  glUseProgram(*geomShader);
  glUniformMatrix4fv(glGetUniformLocation(*geomShader, "mdl2World"), 1, GL_TRUE, mdl2World.m);
  glUniformMatrix4fv(glGetUniformLocation(*geomShader, "world2View"), 1, GL_TRUE, cameraMatrix.m);
  glUniform1f(glGetUniformLocation(*geomShader,"time"), time); 
  //DrawModel(model, *geomShader, "inPosition", "inNormal","inTexCoord");
  glDisable (GL_POLYGON_SMOOTH);
  
}
