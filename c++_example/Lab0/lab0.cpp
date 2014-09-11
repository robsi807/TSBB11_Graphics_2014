
// Converted to MicroGlut and VectorUtils3 2013.
// MicroGLut currently exists for Linux and Mac OS X. On other platforms (e.g. MS Windows)
// you should use FreeGLUT. You will also need GLEW. You may sometimes need to work around
// differences, e.g. additions in MicroGlut that don't exist in FreeGlut.

// gcc lab0.c ../common/*.c -lGL -o lab0 -I../common

#ifdef __APPLE__
	#include <OpenGL/gl3.h>
	#include "../common/mac/MicroGlut.h"
	//uses framework Cocoa
#else
	#include <GL/gl.h>
	#include "../common/linux/MicroGlut.h"
#endif
#include "../common/GL_utilities.h"
#include "../common/VectorUtils3.h"
#include "../common/loadobj.h"
#include "../common/zpr.h"
#include "../common/LoadTGA.h"

#include <iostream>

using namespace std;

//constants
const int initWidth=512,initHeight=512;

// Modify this matrix.
// See below for how it is applied to your model.
mat4 objectExampleMatrix = {{ 1.0, 0.0, 0.0, 0.0,
                            0.0, 1.0, 0.0, 0.0,
                            0.0, 0.0, 1.0, 0.0,
                            0.0, 0.0, 0.0, 1.0}};

mat4 viewMatrix, projectionMatrix;

// Globals
// * Model(s)
Model *bunny;
// * Reference(s) to shader program(s)
GLuint program;
// * Texture(s)
GLuint texture;

void init(void)
{

	dumpInfo();

	// GL inits
	glClearColor(0.2,0.2,0.5,0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	printError("GL inits");
	projectionMatrix = perspective(90, 1.0, 0.1, 1000);

	// Load and compile shader
	program = loadShaders("lab0.vert", "lab0.frag");
	printError("init shader");
	
	// Upload geometry to the GPU:
	bunny = LoadModelPlus("objects/bunnyplus.obj");
	printError("load models");

	// Load textures
	LoadTGATextureSimple("textures/maskros512.tga",&texture);
	printError("load textures");
}


void display(void)
{
	printError("pre display");

	// clear the screen
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	//activate the program, and set its variables
	glUseProgram(program);
	glUniformMatrix4fv(glGetUniformLocation(program, "projectionMatrix"), 1, GL_TRUE, projectionMatrix.m);
	mat4 m = Mult(viewMatrix, objectExampleMatrix);
	glUniformMatrix4fv(glGetUniformLocation(program, "viewMatrix"), 1, GL_TRUE, m.m);

	//draw the model
	DrawModel(bunny, program, "in_Position", "in_Normal", NULL);
	
	printError("display");
	
	glutSwapBuffers();
}

void idle()
{
  // This function is called whenever the computer is idle
  // As soon as the machine is idle, ask GLUT to trigger rendering of a new frame
  glutPostRedisplay();
}

vec3 cam = vec3(0,0,2);
vec3 point = vec3(0,0,0);

int main(int argc, char *argv[])
{
  cout << "Funkar!" << endl;
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA|GLUT_DEPTH|GLUT_DOUBLE);
  glutInitContextVersion(3, 2);
  glutCreateWindow ("Lab 0 - OpenGL 3.2+ Introduction");
  zprInit(&viewMatrix, cam, point);
  glutDisplayFunc(display); 
  glutIdleFunc(idle);
  init ();
  glutMainLoop();
  exit(0);
}

