// http://www.fabiensanglard.net/shadowmapping/index.php
// Win32 support removed. See original if you need it, but you should rather use GLEW or glee.

// Unlike what is stated at the page above, this runs just fine on the Mac, in my case both compiled and from Lightweight IDE.
// So uploading a pre-compiled binary is no problem.

// Reorganized a bit
// Removed most ARB and EXT
// Replaced the clumsy startTranslate/endTranslate with APPLY_ON_BOTH
// (from old non-shader shadow mapping demo).
// Put in more interesting objects.

// 2012: Ported to OpenGL 3.2. APPLY_ON_BOTH replaced by explicit transformations.

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include "MicroGlut.h"
// uses framework Cocoa
#else
#include <GL/gl.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "LoadTGA.h"
#include "VectorUtils3.h"
#include "GL_utilities.h"
#include "loadobj.h"

// ------- Application code --------

// Expressed as float so gluPerspective division returns a float and not 0 (640/480 != 640.0/480.0).
#define RENDER_WIDTH 640.0
#define RENDER_HEIGHT 480.0

// We assign one texture unit in which to store the transformation.
#define TEX_UNIT 0

//Camera position
Point3D p_camera = {32,20,0};

//Camera lookAt
Point3D l_camera = {2,0,-10};

//Light mouvement circle radius
float light_mvnt = 40.0f; // At 30 we get edge artifacts

//Light position
Point3D p_light = {40,20,0};

//Light lookAt
//Point3D l_light = {0,3,-5};
Point3D l_light = {0,3,-10};

// Use to activate/disable projTexShader
GLuint projTexShaderId, plainShaderId;
GLuint projTexMapUniform;

FBOstruct *fbo;

#define NEAR 1.0
#define FAR 100.0
#define W 600
#define H 600

//-----------------------------matrices------------------------------
mat4	modelViewMatrix, textureMatrix;
mat4 projectionMatrix;

void loadShadowShader()
{
  projTexShaderId = loadShaders("VertexShader.vs", "FragmentShader.fs");
  projTexMapUniform = glGetUniformLocation(projTexShaderId,"textureUnit");
  plainShaderId = loadShaders("plain.vs", "plain.fs");
}

// This update only change the position of the light.
void updatePositions(void)
{
  p_light.x = light_mvnt * cos(glutGet(GLUT_ELAPSED_TIME)/1000.0);
  p_light.z = light_mvnt * sin(glutGet(GLUT_ELAPSED_TIME)/1000.0);
}

// Build the transformation sequence for the light source path,
// by copying from the ordinary camera matrices.
void setTextureMatrix(void)
{
  mat4 scaleBiasMatrix;
	
  IdentityMatrix(textureMatrix);
	
  // Scale and bias transform, moving from unit cube [-1,1] to [0,1]
  scaleBiasMatrix = Mult(T(0.5, 0.5, 0.0), S(0.5, 0.5, 1.0));
  textureMatrix = Mult(Mult(scaleBiasMatrix, projectionMatrix), modelViewMatrix);
  // Multiply modelview and transformation matrices
}

// Arrays for ground and the Model references

GLfloat groundcolor[] = {0.3f,0.3f,0.3f,1};
GLfloat ground[] = {
  -35,2,-35,
  -35,2, 15,
  15,2, 15,
  15,2,-35
};
GLuint groundIndices[] = {0, 1, 2, 0, 2, 3};

Model *groundModel, *torusModel, *sphereModel;

void loadObjects(void)
{
  // The shader must be loaded before this is called!
  if (projTexShaderId == 0)
    printf("Warning! Is the shader not loaded?\n");
	
  groundModel = LoadDataToModel(
				ground,
				NULL,
				NULL,
				NULL,
				groundIndices,
				4,
				6);

  torusModel = LoadModelPlus("torus.obj");
  sphereModel = LoadModelPlus("sphere.obj");
}

void drawObjects(GLuint shader)
{
  mat4 mv2, tx2, trans, rot, scale;

  glUniformMatrix4fv(glGetUniformLocation(projTexShaderId, "projectionMatrix"), 1, GL_TRUE, projectionMatrix.m);

  // Ground
  glUniform1f(glGetUniformLocation(projTexShaderId, "shade"), 0.3); // Dark ground
  glUniformMatrix4fv(glGetUniformLocation(projTexShaderId, "modelViewMatrix"), 1, GL_TRUE, modelViewMatrix.m);
  glUniformMatrix4fv(glGetUniformLocation(projTexShaderId, "textureMatrix"), 1, GL_TRUE, textureMatrix.m);
  DrawModel(groundModel, projTexShaderId, "in_Position", NULL, NULL);
	
  glUniform1f(glGetUniformLocation(projTexShaderId, "shade"), 0.9); // Brighter objects
	
  // One torus
  trans = Mult(T(0,4,-16), S(2.0, 2.0, 2.0)); // Apply on both
  mv2 = Mult(modelViewMatrix, trans); // Apply on both
  tx2 = Mult(textureMatrix, trans);
  // Upload both!
  glUniformMatrix4fv(glGetUniformLocation(projTexShaderId, "modelViewMatrix"), 1, GL_TRUE, mv2.m);
  glUniformMatrix4fv(glGetUniformLocation(projTexShaderId, "textureMatrix"), 1, GL_TRUE, tx2.m);
  DrawModel(torusModel, projTexShaderId, "in_Position", NULL, NULL);

  // The other torus
  trans = Mult(Mult(T(0,4,-16), Ry(3.14/2)), S(2.0, 2.0, 2.0)); // Apply on both
  mv2 = Mult(modelViewMatrix, trans);
  tx2 = Mult(textureMatrix, trans);
  // Upload both!
  glUniformMatrix4fv(glGetUniformLocation(projTexShaderId, "modelViewMatrix"), 1, GL_TRUE, mv2.m);
  glUniformMatrix4fv(glGetUniformLocation(projTexShaderId, "textureMatrix"), 1, GL_TRUE, tx2.m);
  DrawModel(torusModel, projTexShaderId, "in_Position", NULL, NULL);
	
  // The sphere
  trans = Mult(T(0,4,-5), S(5.0, 5.0, 5.0));
  trans = Mult(T(0,4,-4), S(5.0, 5.0, 5.0));
  mv2 = Mult(modelViewMatrix, trans); // Apply on both
  tx2 = Mult(textureMatrix, trans);
  // Upload both!
  glUniformMatrix4fv(glGetUniformLocation(projTexShaderId, "modelViewMatrix"), 1, GL_TRUE, mv2.m);
  glUniformMatrix4fv(glGetUniformLocation(projTexShaderId, "textureMatrix"), 1, GL_TRUE, tx2.m);
  DrawModel(sphereModel, projTexShaderId, "in_Position", NULL, NULL);
}

void renderScene(void) 
{
  // Change light positions
  updatePositions();
	
  // Setup projection matrix
  projectionMatrix = perspective(45, RENDER_WIDTH/RENDER_HEIGHT, 10, 4000);
	
  // Setup the modelview from the light source
  modelViewMatrix = lookAt(p_light.x, p_light.y, p_light.z,
			   l_light.x, l_light.y, l_light.z, 0,1,0);
  // Using the result from lookAt, add a bias to position the result properly in texture space
  setTextureMatrix();

  // 1. Render scene to FBO

  useFBO(fbo, NULL, NULL);
  glViewport(0,0,RENDER_WIDTH,RENDER_HEIGHT);
  glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_TRUE); // Depth only
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  //Using the simple shader
  glUseProgram(projTexShaderId);
  glUniform1i(projTexMapUniform,TEX_UNIT);
  glActiveTexture(GL_TEXTURE0 + TEX_UNIT);
  glBindTexture(GL_TEXTURE_2D,0);
	
  drawObjects(plainShaderId);
  //	glFlush(); Causes problems on the Mac, but might be needed on other platforms
	
  //2. Render from camera.
  // Now rendering from the camera POV

  useFBO(NULL, fbo, NULL);
	
  glViewport(0,0,RENDER_WIDTH,RENDER_HEIGHT);
	
  //Enabling color write (previously disabled for light POV z-buffer rendering)
  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	
  // Clear previous frame values
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  //Using the projTex shader
  glUseProgram(projTexShaderId);
  glUniform1i(projTexMapUniform,TEX_UNIT);
  glActiveTexture(GL_TEXTURE0 + TEX_UNIT);
  glBindTexture(GL_TEXTURE_2D,fbo->depth);
	
  // Setup the modelview from the camera
  modelViewMatrix = lookAt(p_camera.x, p_camera.y, p_camera.z, 
			   l_camera.x, l_camera.y, l_camera.z, 0,1,0);
	
  glCullFace(GL_BACK);
  drawObjects(projTexShaderId);
	
  glutSwapBuffers();
}

void processNormalKeys(unsigned char key, int x, int y)
{
  if (key == 27) 
    exit(0);
}

int main(int argc, char** argv)
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
  //glutInitWindowPosition(100,100);
  glutInitWindowSize(RENDER_WIDTH,RENDER_HEIGHT);
  glutInitContextVersion(3, 2);
  glutCreateWindow("Shadow mapping demo");
	
  loadShadowShader();
  loadObjects();
  fbo = initFBO2(RENDER_WIDTH,RENDER_HEIGHT, 0, 1);
	
  glEnable(GL_DEPTH_TEST);
  glClearColor(0,0,0,1.0f);
  glEnable(GL_CULL_FACE);
	
  glutDisplayFunc(renderScene);
  glutRepeatingTimerFunc(20); // MicroGlut only
  glutKeyboardFunc(processNormalKeys);
	
  glutMainLoop();
  exit(0);
}
