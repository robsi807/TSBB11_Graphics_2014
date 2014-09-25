
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

using namespace std;

World* world;

// Should be a member of world
//GLfloat t = 0;

void init(void)
{

  // GL inits
  glClearColor(0.2,0.2,0.5,0);
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  printError("GL inits");
  
  world = new World();

  // Should be moved to world
  //lSource = LoadModelPlus("../objects/groundsphere.obj");
  //sphere1 = LoadModelPlus("../objects/groundsphere.obj");

}

void display(void)
{
  //t = (GLfloat)glutGet(GLUT_ELAPSED_TIME) / 3000;
  
  world->draw();

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
  glutPostRedisplay();
  world->camera->handleKeyPress();
  //glutPassiveMotionFunc(mouse);
  glutTimerFunc(20, &timer, i);
}

int main(int argc, char **argv)
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
  glutInitContextVersion(3, 2);
  glutInitWindowSize(1024,860); //Change! Do not hard code!
  glutCreateWindow("TSBK03 C++");
  glutDisplayFunc(display);
  init();
  glutTimerFunc(20, &timer, 0);
  glutPassiveMotionFunc(mouse);

  glutMainLoop();
  delete world;
  exit(0);
}
