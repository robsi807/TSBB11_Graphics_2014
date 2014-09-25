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
#include "../common/LoadTGA.h"

#include "World.h"


#include <cmath> 
#include <iostream>

using namespace std;

#define PI 3.1415


World* world;

GLfloat t = 0;

vec3 lightSource = vec3(50.0f, 100.0f, 50.0f);
GLfloat specularExponent = 50;

// vertex array object
//Model *lSource,*sphere1;
// Reference to shader world->phongShader

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

  // Load light. THIS IS NOT WORKING YET! CHANGES IN SHADER NEEDED
  glUniform3fv(glGetUniformLocation(world->phongShader, "lightSource"), 1, &lightSource.x); //&
  glUniform1fv(glGetUniformLocation(world->phongShader, "specularExponent"), 1, &specularExponent);

  // Place model at light source
  //lSource = LoadModelPlus("../objects/groundsphere.obj");
  //sphere1 = LoadModelPlus("../objects/groundsphere.obj");

}

void display(void)
{
  t = (GLfloat)glutGet(GLUT_ELAPSED_TIME) / 3000;
  
  world->renderWorld();
  

/*
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
 */
  glutSwapBuffers();
}

void mouse(int x, int y)
{
  //printf("%d %d\n", x, y);
  world->camera->handleMouse(x,y);
}

void timer(int i)
{
  glutTimerFunc(20, &timer, i);
  glutPostRedisplay();
  world->camera->handleKeyPress();
  glutPassiveMotionFunc(mouse);
}

int main(int argc, char **argv)
{
  cout << "Funkar nu!" << endl;
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
  glutInitContextVersion(3, 2);
  glutInitWindowSize(1024,860);
  glutCreateWindow("TSBK03 C++");
  glutDisplayFunc(display);
  init();
  glutTimerFunc(20, &timer, 0);

  glutPassiveMotionFunc(mouse);

  glutMainLoop();
  delete world;
  exit(0);
}
