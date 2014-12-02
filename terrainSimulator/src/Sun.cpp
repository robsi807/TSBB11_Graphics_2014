
#include "Sun.h"

GLfloat square[] = {-1,-1,0,
		      -1,1,0,
		      1,1,0,
		      1,-1,0};
  GLfloat squareTexCoord[] = {0, 0,
			      0, 1,
			      1, 1,
			      1, 0};
  GLuint squareIndices[] = {0, 1, 2, 0, 2, 3};

Sun::Sun(GLuint* sunShader, mat4 projectionMatrix)
{
  fbo2 = initFBO(Camera::SCREEN_WIDTH,Camera::SCREEN_HEIGHT, 0);
  fbo3 = initFBO(Camera::SCREEN_WIDTH,Camera::SCREEN_HEIGHT, 0);
  distance = 400.0;
  specularExponent = 2.0;
  lightDir = Normalize(vec3(0.0, 2.0, 2.0))*distance;
  lightDirNight = Normalize(vec3(0.0f, 2.0f, 0.0f));

  

  squareModel = LoadDataToModel2(square,NULL,squareTexCoord,NULL,squareIndices,4,6);

  textureadder = loadShaders("shaders/blooming/textureadder.vert", "shaders/blooming/textureadder.frag");  
  lowpass = loadShaders("shaders/blooming/lowpass.vert", "shaders/blooming/lowpass.frag");
  truncate = loadShaders("shaders/blooming/truncate.vert","shaders/blooming/truncate.frag");
  
  shader = sunShader;
  model = LoadModelPlus("../objects/groundsphere.obj");
 
  glUseProgram(*shader);
  glUniformMatrix4fv(glGetUniformLocation(*shader, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);
}

void Sun::update(GLfloat time)
{
  lightDir = Normalize(vec3(0.0, 2.0*cos(time), 2.0*sin(time)))*distance;
  position.y = 2.0*cos(time);
  position.z = 2.0*sin(time);
  position = Normalize(position)*distance;
}

void Sun::bloom(FBOstruct *fbo1)
{
  // Truncate light on bunny
  glUseProgram(truncate);
  useFBO(fbo2, fbo1, 0L);
  glClearColor(0.0, 0.0, 0.0, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glDisable(GL_CULL_FACE);
  glDisable(GL_DEPTH_TEST);

  glUniform1i(glGetUniformLocation(truncate, "texUnit"), 0);
  
  DrawModel(squareModel, truncate, "in_Position", NULL, "in_TexCoord");
	
  // Low pass filter onlylight bunny
  glUseProgram(lowpass);

  for(int i = 0; i < 10; i++)
    {
    useFBO(fbo3, fbo2, 0L);
    glClearColor(0.0, 0.0, 0.0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

    glUniform1i(glGetUniformLocation(lowpass, "texUnit"), 0);
	
    DrawModel(squareModel, lowpass, "in_Position", NULL, "in_TexCoord");

    useFBO(fbo2, fbo3, 0L);
    glClearColor(0.0, 0.0, 0.0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

    glUniform1i(glGetUniformLocation(lowpass, "texUnit"), 0);
	
    DrawModel(squareModel, lowpass, "in_Position", NULL, "in_TexCoord");
  }

  useFBO(0L,fbo1,fbo3);
  glUseProgram(textureadder);
  glUniform1i(glGetUniformLocation(textureadder, "texUnit1"), 0);
  glUniform1i(glGetUniformLocation(textureadder, "texUnit2"), 1);
  DrawModel(squareModel, textureadder, "in_Position", NULL, "in_TexCoord");
  
}


void Sun::draw(mat4 cameraMatrix, GLfloat time)
{

  glDisable(GL_DEPTH_TEST);
  glUseProgram(*shader);
  
  float sunScale = 10.0;

  mat4 sunSize = S(sunScale,sunScale,sunScale);
  mat4 sunTranslation = T(position.x,position.y,position.z);
  mat4 modelViewSun = Mult(sunTranslation,sunSize);
  glUniformMatrix4fv(glGetUniformLocation(*shader, "mdl2World"), 1, GL_TRUE, modelViewSun.m);
  mat4 world2ViewSun = cameraMatrix;
  world2ViewSun.m[3] = 0;
  world2ViewSun.m[7] = 0;
  world2ViewSun.m[11] = 0;
  glUniformMatrix4fv(glGetUniformLocation(*shader, "world2View"), 1, GL_TRUE, world2ViewSun.m);
  glUniform1f(glGetUniformLocation(*shader, "alpha"), 1.0);
  DrawModel(model, *shader, "inPosition", NULL,NULL);
  
  glEnable(GL_DEPTH_TEST);
}
