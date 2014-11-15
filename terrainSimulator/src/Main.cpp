
#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include "../common/mac/MicroGlut.h"
// Linking hint for Lightweight IDE
// uses framework Cocoa
#else
#include <GL/gl.h>
#include "../common/linux/MicroGlut.h"
#include <X11/Xlib.h> // Moved! XInitThreads() does not exist in the Mac implementation.
#endif

#include "GL_utilities.h"
#include "VectorUtils3.h"
#include "LoadTGA.h"

#include "World.h"
#include "Camera.h"

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

}

void display(void)
{
  world->update();
  world->draw();
  glutSwapBuffers();
}

void mouse(int x, int y)
{
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
  #ifdef __linux__
  XInitThreads(); // Must be moved! Does not exist in the Mac implementation.
  #endif
  glutInitWindowSize(Camera::SCREEN_WIDTH,Camera::SCREEN_HEIGHT);
  glutCreateWindow("THE GENERATOR");
  glutDisplayFunc(display);
  init();
  glutTimerFunc(20, &timer, 0);
  glutPassiveMotionFunc(mouse);

  glutMainLoop();
  delete world;
  exit(0);
}
