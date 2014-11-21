//________________________________skybox.cpp________________________________//
// Filename: skybox.cpp         				            //
// Description: .cpp file for the skybox class. Includes class functions.   //
// Author: Carl Stejmar	(creator)                                           //
// Date: 2014-09-17       				                    //
//__________________________________________________________________________//

#include "Skybox.h"

Skybox::Skybox(GLuint* skyboxShader,mat4 projectionMatrix, const char *imagePath)
{
  //bottomColor = vec3(1,1,1);
  //topColor = vec3(.6,.87,0.99);
  shader = skyboxShader;
  model = LoadModelPlus("../objects/skyboxbig.obj");

  loadImages(imagePath);
  generateCubeMap();
  
  mat4 transSkybox = T(0,0,0);
  glUseProgram(*shader);
  glUniform1i(glGetUniformLocation(*shader, "cubeMap"), 16); // Texture unit 16: skybox
  glUniformMatrix4fv(glGetUniformLocation(*shader, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);
  glUniformMatrix4fv(glGetUniformLocation(*shader, "mdl2World"), 1, GL_TRUE, transSkybox.m);
}

// Starts at GL_TEXTURE10 (to 15). (This could be a private member instead, set when constructing the class object.)
void Skybox::loadImages(const char *imagePath) //imagePath = "textures/skybox/sky%d.tga"
{
  int j = 6; //(sizeof(texture)/sizeof(texture[0]))+1;
  //printf("Number of elements: %i \n", j);

  for(int i = 0; i < j; i++) 
    {
      char file[80]; // Godtycklig längd, bara strängen får plats
      sprintf(file, imagePath, i);
      
      glActiveTexture(GL_TEXTURE10 + i);
      LoadTGATexture(file, &texture[i]);
    }
}

// Starts at GL_TEXTURE10 (to 15)
void Skybox::generateCubeMap()
{
  int j = 6; //(sizeof(texture)/sizeof(texture[0]))+1;
  //printf("Number of elements2: %i \n", j);

  glActiveTexture(GL_TEXTURE10 + j);

  printf("BPP for texture[1]: %i \n", texture[1].bpp);

  for(int i = 0; i < j; i++)
    {
      GLuint format = GL_RGBA; // Set The Default GL Mode To RBGA (32 BPP)
      if (texture[i].bpp == 24) // Was The TGA 24 Bits?
	{
	  format=GL_RGB;
	}

      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
		   0, format, texture[i].w, texture[i].h, 0, 
		   format, GL_UNSIGNED_BYTE, texture[i].imageData);
    }

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void Skybox::updateColors(GLfloat time){
  topColor.x = 0.6*((1 + cos(time))/2.0);
  topColor.y = 0.87*((1 + cos(time))/2.0);
  topColor.z = 0.99*((1 + cos(time))/2.0);

  bottomColor.x = ((1 + cos(time))/2.0);
  bottomColor.y = ((1 + cos(time))/2.0);
  bottomColor.z = ((1 + cos(time))/2.0);
}


void Skybox::draw(mat4 cameraMatrix, GLfloat time)
{
  updateColors(time);

  glDisable(GL_DEPTH_TEST);
  glUseProgram(*shader);

  mat4 world2ViewSky = cameraMatrix;
  world2ViewSky.m[3] = 0;
  world2ViewSky.m[7] = 0;
  world2ViewSky.m[11] = 0;

  glUniformMatrix4fv(glGetUniformLocation(*shader, "world2View"), 1, GL_TRUE, world2ViewSky.m);
  glUniform3fv(glGetUniformLocation(*shader, "bottomColor"), 1, &bottomColor.x);
  glUniform3fv(glGetUniformLocation(*shader, "topColor"), 1, &topColor.x);
  glUniform1f(glGetUniformLocation(*shader, "time"), time);
  DrawModel(model, *shader,"vertices",NULL,NULL);
  
  glEnable(GL_DEPTH_TEST);
}
