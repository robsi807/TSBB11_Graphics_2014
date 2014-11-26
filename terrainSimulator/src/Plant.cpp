#include "Plant.h"

// Static variables
Model* Plant::model;
GLuint* Plant::shader;
GLuint* Plant::geomShader;

// Initialize static variables
void Plant::initPlants(GLuint *shade,GLuint *geoShade,Model* mod){
   shader = shade;
   geomShader = geoShade;
   model = mod;
}

Plant::Plant(vec3 pos,float yRot,float scaling,vec3 terrainPos){  
  position = pos;
  mat4 transTerrain = T(terrainPos.x,0.0,terrainPos.z);  
  mat4 trans = T(position.x,position.y,position.z);
  mat4 rot = Ry(yRot);
  mat4 scale = S(scaling,scaling,scaling);
  mdl2World =  transTerrain * trans;
}


void Plant::draw(mat4 cameraMatrix,float time){
  
  glUseProgram(*shader);
  float specularExponent = 2.0;
  glUniform1fv(glGetUniformLocation(*shader, "specularExponent"), 1, &specularExponent);
  glUniformMatrix4fv(glGetUniformLocation(*shader, "mdl2World"), 1, GL_TRUE, mdl2World.m);
  glUniformMatrix4fv(glGetUniformLocation(*shader, "world2View"), 1, GL_TRUE, cameraMatrix.m);
  DrawModel(model, *shader, "inPosition", "inNormal","inTexCoord"); 

  glEnable (GL_POLYGON_SMOOTH);
  glUseProgram(*geomShader);
  glUniformMatrix4fv(glGetUniformLocation(*geomShader, "mdl2World"), 1, GL_TRUE, mdl2World.m);
  glUniformMatrix4fv(glGetUniformLocation(*geomShader, "world2View"), 1, GL_TRUE, cameraMatrix.m);
  glUniform1f(glGetUniformLocation(*geomShader,"time"), time); 
  DrawModel(model, *geomShader, "inPosition", "inNormal","inTexCoord");
  glDisable (GL_POLYGON_SMOOTH);
  
}
