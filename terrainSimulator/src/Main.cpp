// Lab 4, terrain generation

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include "../common/mac/MicroGlut.h"
// Linking hint for Lightweight IDE
// uses framework Cocoa
#else
#include <GL/gl.h>
#include "../common/linux/MicroGlut.h"
#endif

#include "../common/GL_utilities.h"
#include "../common/VectorUtils3.h"
#include "../common/loadobj.h"
//#include "../common/zpr.h"
#include "../common/LoadTGA.h"

#include "Skybox.h"
#include "Camera.h"
#include "TerrainPatch.h"


#include <cmath> // Should be changed to <cmath>!
#include <iostream>

using namespace std;

#define PI 3.1415


World* world;

GLfloat t = 0;

const GLfloat lightSource[3] = {50.0f, 100.0f, 50.0f}; //Point3D
GLfloat specularExponent = 50;

// vertex array object
Model *lSource,*sphere1;
// Reference to shader world->phongShader
GLuint tex1, tex2;

void init(void)
{

  // GL inits
  glClearColor(0.2,0.2,0.5,0);
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  printError("GL inits");
  
  world = new World();

  glUseProgram(world->phongShader);

  glUniformMatrix4fv(glGetUniformLocation(world->phongShader, "projMatrix"), 1, GL_TRUE, world->camera->projectionMatrix.m);
  glUniform1i(glGetUniformLocation(world->phongShader, "tex"), 0); // Texture unit 0
  LoadTGATextureSimple("../textures/grass.tga", &tex1);

  // Load light. THIS IS NOT WORKING YET! CHANGES IN SHADER NEEDED
  glUniform3fv(glGetUniformLocation(world->phongShader, "lightSource"), 1, lightSource); //&
  glUniform1fv(glGetUniformLocation(world->phongShader, "specularExponent"), 1, &specularExponent);

  // Load terrain data



  printError("init terrain");

  // Place model at light source
  lSource = LoadModelPlus("../objects/groundsphere.obj");
  sphere1 = LoadModelPlus("../objects/groundsphere.obj");

  //Skybox
  glUseProgram(skyboxProgram);

  //skybox.loadImages("textures/skybox/sky%d.tga");
  //skybox.generateCubeMap();
  skybox = Skybox(skyboxProgram);
  skybox.init(cam.projectionMatrix, "../textures/skybox/sky%d.tga");

  //printf("BPP for texture[1] after init: %i \n", skybox.texture[1].bpp); // private
  printError("Error: init skybox");
}

void display(void)
{
  cam.update();
  t = (GLfloat)glutGet(GLUT_ELAPSED_TIME) / 3000;
  mat4 modelView = IdentityMatrix();
  mat4 total = Mult(cam.cameraMatrix,modelView);
  // clear the screen
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  //Disable Z-buffer before drawing skybox
  glDisable(GL_DEPTH_TEST);
  glUseProgram(skyboxProgram);
  skybox.draw(cam.cameraMatrix);
  glEnable(GL_DEPTH_TEST);
  printError("Error in display skybox");

  printError("pre display");

  glUseProgram(world->phongShader);
  glUniformMatrix4fv(glGetUniformLocation(world->phongShader, "mdl2World"), 1, GL_TRUE, modelView.m);
  glUniformMatrix4fv(glGetUniformLocation(world->phongShader, "world2View"), 1, GL_TRUE, cam.cameraMatrix.m);
  glUniformMatrix4fv(glGetUniformLocation(world->phongShader, "mdlMatrix"), 1, GL_TRUE, total.m);
  glBindTexture(GL_TEXTURE_2D, tex1);		// Bind Our Texture tex1
  DrawModel(terrainPatch->geometry, world->phongShader, "inPosition", "inNormal", "inTexCoord"); // "inNormal"

  // Light source sphere
  modelView = T(50,105,50);
  glUniformMatrix4fv(glGetUniformLocation(world->phongShader, "mdl2World"), 1, GL_TRUE, modelView.m);
  DrawModel(lSource,world->phongShader,"inPosition","inNormal","inTexCoord");
  printError("display 1");

  // Sphere1
  float xs,zs,ys;
  int texWidth = ttex.width;
  xs = 70+20*cos(t);
  zs = 70+20*sin(t);
  ys = terrainPatch->calcHeight(xs,zs,texWidth);
  modelView = T(xs,ys,zs);
  glUniformMatrix4fv(glGetUniformLocation(world->phongShader, "mdl2World"), 1, GL_TRUE, modelView.m);
  DrawModel(sphere1,world->phongShader,"inPosition","inNormal","inTexCoord");
  printError("display 2");

  glutSwapBuffers();
}

void mouse(int x, int y)
{
  //printf("%d %d\n", x, y);
  cam.handleMouse(x,y);
}

void timer(int i)
{
  glutTimerFunc(20, &timer, i);
  glutPostRedisplay();
  cam.handleKeyPress();
  glutPassiveMotionFunc(mouse);
}

int main(int argc, char **argv)
{
  cout << "Funkar nu!" << endl;
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
  glutInitContextVersion(3, 2);
  glutInitWindowSize(WIDTH,HEIGHT);
  glutCreateWindow("TSBK03 C++");
  glutDisplayFunc(display);
  init();
  glutTimerFunc(20, &timer, 0);

  glutPassiveMotionFunc(mouse);

  glutMainLoop();
  delete terrainPatch;
  exit(0);
}
