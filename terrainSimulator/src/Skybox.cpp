//________________________________skybox.cpp________________________________//
// Filename: skybox.cpp         				            //
// Description: .cpp file for the skybox class. Includes class functions.   //
// Author: Carl Stejmar	(creator)                                           //
// Date: 2014-09-17       				                    //
//__________________________________________________________________________//

#include "Skybox.h"

Skybox::Skybox(GLuint* skyboxShader,mat4 projectionMatrix, const char *imagePath)
{
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

// Lägg in färger i texture[] istället för bilddata (men hur får vi -> Interpolerade färger?)
// GLfloat colors[36*3] = {
//   1.0, 0.0, 0.0,	// Red
//   1.0, 0.0, 0.0,	// Red
//   1.0, 0.0, 0.0,	// Red
//   1.0, 0.0, 0.0,	// Red
//   1.0, 0.0, 0.0,	// Red
//   1.0, 0.0, 0.0,	// Red

//   0.0, 1.0, 0.0,	// Green
//   0.0, 1.0, 0.0,	// Green
//   0.0, 1.0, 0.0,	// Green
//   0.0, 1.0, 0.0,	// Green
//   0.0, 1.0, 0.0,	// Green
//   0.0, 1.0, 0.0,	// Green

//   0.0, 0.0, 1.0,	// Blue
//   0.0, 0.0, 1.0,	// Blue
//   0.0, 0.0, 1.0,	// Blue
//   0.0, 0.0, 1.0,	// Blue
//   0.0, 0.0, 1.0,	// Blue
//   0.0, 0.0, 1.0,	// Blue

//   0.0, 1.0, 1.0,	// Cyan
//   0.0, 1.0, 1.0,	// Cyan
//   0.0, 1.0, 1.0,	// Cyan
//   0.0, 1.0, 1.0,	// Cyan
//   0.0, 1.0, 1.0,	// Cyan
//   0.0, 1.0, 1.0,	// Cyan

//   1.0, 0.0, 1.0,	// Magenta
//   1.0, 0.0, 1.0,	// Magenta
//   1.0, 0.0, 1.0,	// Magenta
//   1.0, 0.0, 1.0,	// Magenta
//   1.0, 0.0, 1.0,	// Magenta
//   1.0, 0.0, 1.0,	// Magenta

//   1.0, 1.0, 0.0,	// Yellow
//   1.0, 1.0, 0.0,	// Yellow
//   1.0, 1.0, 0.0,	// Yellow
//   1.0, 1.0, 0.0,	// Yellow
//   1.0, 1.0, 0.0,	// Yellow
//   1.0, 1.0, 0.0,	// Yellow
// };

// unsigned int colorBufferObjID;
// glGenBuffers(1, &colorBufferObjID);

// // VBO for color data
// glBindBuffer(GL_ARRAY_BUFFER, colorBufferObjID);
// glBufferData(GL_ARRAY_BUFFER, 36*3*sizeof(GLfloat), colors, GL_STATIC_DRAW);
// glVertexAttribPointer(glGetAttribLocation(program, "in_Color"), 3, GL_FLOAT, GL_FALSE, 0, 0);
// glEnableVertexAttribArray(glGetAttribLocation(program, "in_Color"));
	
// glUniformMatrix4fv(glGetUniformLocation(program, "translationMatrix"), 1, GL_TRUE, translationMatrix);
// glUniformMatrix4fv(glGetUniformLocation(program, "rotationMatrix"), 1, GL_TRUE, rotationMatrix);
// glUniformMatrix4fv(glGetUniformLocation(program, "projMatrix"), 1, GL_TRUE, projectionMatrix);

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


void Skybox::draw(mat4 cameraMatrix)
{
  glDisable(GL_DEPTH_TEST);
  glUseProgram(*shader);

  mat4 world2ViewSky = cameraMatrix;
  world2ViewSky.m[3] = 0;
  world2ViewSky.m[7] = 0;
  world2ViewSky.m[11] = 0;

  glUniformMatrix4fv(glGetUniformLocation(*shader, "world2View"), 1, GL_TRUE, world2ViewSky.m);
  DrawModel(model, *shader,"vertices",NULL,NULL);
  
  glEnable(GL_DEPTH_TEST);
}
