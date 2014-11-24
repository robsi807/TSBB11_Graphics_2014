#include "Plant.h"

Plant::Plant(GLuint *shade,GLuint *geoShade,Model* mod,vec3 pos,float yRot,float scaling)
  : shader(shade),geomShader(geoShade),model(mod){  
    
  mat4 trans = T(position.x,position.y,position.z);
  mat4 rot = Ry(yRot);
  mat4 scale = S(scaling,scaling,scaling);
  mdl2World = scale * rot * trans;
}


void Plant::draw(Camera* cam,float time){
  
  mat4 cameraMatrix = cam->cameraMatrix;
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
