#include "Plant.h"

// Static variables
Model* Plant::bushModel;
Model* Plant::bushTrunkModel;
Model* Plant::treeModel;
Model* Plant::treeTrunkModel;
GLuint* Plant::shader;
GLuint* Plant::geomBushShader;
GLuint* Plant::geomTreeShader;
GLuint Plant::woodTexture;

// Initialize static variables
void Plant::initPlants(GLuint *shade,GLuint *geoBushShade,GLuint *geoTreeShade){
  shader = shade;
  geomBushShader = geoBushShade;
  geomTreeShader = geoTreeShade;
   
  treeModel = LoadModelPlus("../objects/tree2.obj");
  treeTrunkModel = LoadModelPlus("../objects/tree2trunk.obj");

  bushModel = LoadModelPlus("../objects/BushLow.obj");
  bushTrunkModel = NULL;
	glUseProgram(*shader);
  glActiveTexture(GL_TEXTURE0+7);
  LoadTGATextureSimple("../textures/tree1_1024.tga",&woodTexture);
  glUniform1i(glGetUniformLocation(*shader,"tex"),7);
}   

//Constructor
Plant::Plant(vec3 pos,float yRot,float scaling,vec3 terrainPos,PlantType plantType){  
  position = pos;
  globalPosition = pos + terrainPos;
  scale = scaling;
  type = plantType;
  mat4 transTerrain = T(terrainPos.x,0.0,terrainPos.z);  
  mat4 trans = T(position.x,position.y,position.z);
  mat4 rot = Ry(yRot);
  mat4 scaleMat = S(scaling/1.5,scaling,scaling/1.5); // Scale height more
  mdl2World =  transTerrain * trans * rot * scaleMat;
}

//Draws the plant
void Plant::draw(mat4 cameraMatrix,vec3 camPos,float time){
  
  GLuint* geomShader;
  Model *model,*trunkModel;
  if(type == Tree){
    geomShader = geomTreeShader;
    model = treeModel;
    trunkModel = treeTrunkModel;
  } else {
    geomShader = geomBushShader;
    model = bushModel;
    trunkModel = NULL; //bushTrunkModel;
  }

  glUseProgram(*shader);
  float specularExponent = 2.0;
  glUniform1fv(glGetUniformLocation(*shader, "specularExponent"), 1, &specularExponent);
  glUniformMatrix4fv(glGetUniformLocation(*shader, "mdl2World"), 1, GL_TRUE, mdl2World.m);
  glUniformMatrix4fv(glGetUniformLocation(*shader, "world2View"), 1, GL_TRUE, cameraMatrix.m);
  glBindTexture(GL_TEXTURE_2D,woodTexture);
  DrawModel(model, *shader, "inPosition", "inNormal","inTexCoord"); 
  if(trunkModel != NULL){
    DrawModel(trunkModel,*shader,"inPosition", "inNormal","inTexCoord");
  }

	// Check if distance too large
	float maxDist = 1000.0;
	if(Norm(camPos-globalPosition) < maxDist){
		glEnable (GL_POLYGON_SMOOTH);
		glUseProgram(*geomShader);
		glUniformMatrix4fv(glGetUniformLocation(*geomShader, "mdl2World"), 1, GL_TRUE, mdl2World.m);
		glUniformMatrix4fv(glGetUniformLocation(*geomShader, "world2View"), 1, GL_TRUE, cameraMatrix.m);
		glUniform1f(glGetUniformLocation(*geomShader,"time"), time); 
		DrawModel(model, *geomShader, "inPosition", "inNormal","inTexCoord");
		glDisable (GL_POLYGON_SMOOTH);
  }
}
