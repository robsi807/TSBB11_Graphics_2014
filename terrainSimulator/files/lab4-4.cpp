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

#include "skybox.h"

#include <math.h>
#include <iostream>

using namespace std;

// Perspective
#define near 0.8
#define far 250.0
#define right 0.5
#define left -0.5
#define top 0.5
#define bottom -0.5

#define PI 3.1415

#define WIDTH 1024
#define HEIGHT 860

// Skybox
Skybox skybox;
//TextureData texture[6];
//GLuint cubeMap;
GLuint skyboxProgram;
Model *mSkybox;

// Movement variables
vec3 p = vec3(0,0,0);
vec3 l = vec3(0,0,0);
vec3 v = vec3(0,0,0);

GLfloat constSpeed = 0.5;
GLfloat t = 0;

mat4 projectionMatrix;

const GLfloat lightSource[3] = {50.0f, 100.0f, 50.0f}; //Point3D
GLfloat specularExponent = 50;

vec3 calcNormal(vec3 v0, vec3 v1, vec3 v2)
{
  vec3 n = CrossProduct(VectorSub(v1,v0),VectorSub(v2,v0));
  if(n.y < 0)
    n = ScalarMult(n,-1);
  return Normalize(n);
}

Model* GenerateTerrain(TextureData *tex)
{
  int vertexCount = tex->width * tex->height;
  int triangleCount = (tex->width-1) * (tex->height-1) * 2;
  int x, z;
	
  GLfloat *vertexArray = (GLfloat *)malloc(sizeof(GLfloat) * 3 * vertexCount);
  GLfloat *normalArray = (GLfloat *)malloc(sizeof(GLfloat) * 3 * vertexCount);
  GLfloat *texCoordArray = (GLfloat *)malloc(sizeof(GLfloat) * 2 * vertexCount);
  GLuint *indexArray = (GLuint *)malloc(sizeof(GLuint) * triangleCount*3);

  float hScale = 20.0;
	
  printf("bpp %d\n", tex->bpp);
  for (x = 0; x < tex->width; x++)
    for (z = 0; z < tex->height; z++)
      {
	vec3 n = {0.0,1.0,0.0};
	if(!((x==0)||(x==tex->width - 1)||(z == 0)||(z==tex->height - 1)))
	  {
	    float x0 = (float)(x-1);
	    float z0 = (float)(z-1);

	    float x1 = (float)x;
	    float z1 = (float)z;
	    
	    float x2 = (float)(x+1);
	    float z2 = (float)(z+1);

	    float y00 = tex->imageData[((int)x0 + (int)z0 * tex->width) * (tex->bpp/8)] / hScale;
	    float y01 = tex->imageData[((int)x1 + (int)z0 * tex->width) * (tex->bpp/8)] / hScale;
	    //float y02 = tex->imageData[((int)x2 + (int)z0 * tex->width) * (tex->bpp/8)] / hScale;

	    float y10 = tex->imageData[((int)x0 + (int)z1 * tex->width) * (tex->bpp/8)] / hScale;
	    float y11 = tex->imageData[((int)x1 + (int)z1 * tex->width) * (tex->bpp/8)] / hScale;
	    float y12 = tex->imageData[((int)x2 + (int)z1 * tex->width) * (tex->bpp/8)] / hScale;

	    //float y20 = tex->imageData[((int)x0 + (int)z2 * tex->width) * (tex->bpp/8)] / hScale;
	    float y21 = tex->imageData[((int)x1 + (int)z2 * tex->width) * (tex->bpp/8)] / hScale;
	    float y22 = tex->imageData[((int)x2 + (int)z2 * tex->width) * (tex->bpp/8)] / hScale;
	    
	    vec3 p00 = {x0,y00,z0};
	    vec3 p01 = {x1,y01,z0};
	    //vec3 p02 = {x2,y02,z0};
	    vec3 p10 = {x0,y10,z1};
	    vec3 p11 = {x1,y11,z1};
	    vec3 p12 = {x2,y12,z1};
	    //vec3 p20 = {x0,y20,z2};
	    vec3 p21 = {x1,y21,z2};
	    vec3 p22 = {x2,y22,z2};

	    vec3 n0 = calcNormal(p11,p10,p00);
	    vec3 n1 = calcNormal(p11,p00,p01);
	    vec3 n2 = calcNormal(p11,p01,p12);
	    vec3 n3 = calcNormal(p11,p12,p22);
	    vec3 n4 = calcNormal(p11,p21,p22);
	    vec3 n5 = calcNormal(p11,p10,p21);
	    
	    n = VectorAdd(n0,n1);
	    n = VectorAdd(n,n2);
	    n = VectorAdd(n,n3);
	    n = VectorAdd(n,n4);
	    n = VectorAdd(n,n5);
	    n = Normalize(n);
	    //Normalize(ScalarMult(VectorAdd(VectorAdd(n0,n1),VectorAdd(VectorAdd(n2,n3),VectorAdd(n4,n5))),mean));
	    //printf("x: %3.2f y: %3.2f z: %3.2f \n", n.x,n.y,n.z);
	    // printf(cout << "x: " << n.x  << " y: " << n.y << " z: " << n.z << endl;
	  }

	//	printf("y: %3.2f \n",  tex->imageData[(x + z * tex->width) * (tex->bpp/8)] / hScale);
	vertexArray[(x + z * tex->width)*3 + 0] = x / 1.0;
	vertexArray[(x + z * tex->width)*3 + 1] = tex->imageData[(x + z * tex->width) * (tex->bpp/8)] / hScale;
	vertexArray[(x + z * tex->width)*3 + 2] = z / 1.0;
	// Normal vectors. You need to calculate these.
	normalArray[(x + z * tex->width)*3 + 0] = n.x; //(y1-y0)*(z2-z0)-(y2-y0)*(z1-z0); //0.0;
	normalArray[(x + z * tex->width)*3 + 1] = n.y; //(z1-z0)*(x2-x0)-(z2-z0)*(x1-x0); //1.0;
	normalArray[(x + z * tex->width)*3 + 2] =n.z; //(x1-x0)*(y2-y0)-(x2-x0)*(y1-y0); //0.0;
	// Texture coordinates. You may want to scale them.
	texCoordArray[(x + z * tex->width)*2 + 0] = x; // (float)x / tex->width;
	texCoordArray[(x + z * tex->width)*2 + 1] = z; // (float)z / tex->height;
      }
  for (x = 0; x < tex->width-1; x++)
    for (z = 0; z < tex->height-1; z++)
      {
	// Triangle 1
	indexArray[(x + z * (tex->width-1))*6 + 0] = x + z * tex->width;
	indexArray[(x + z * (tex->width-1))*6 + 1] = x + (z+1) * tex->width;
	indexArray[(x + z * (tex->width-1))*6 + 2] = x+1 + z * tex->width;
	// Triangle 2
	indexArray[(x + z * (tex->width-1))*6 + 3] = x+1 + z * tex->width;
	indexArray[(x + z * (tex->width-1))*6 + 4] = x + (z+1) * tex->width;
	indexArray[(x + z * (tex->width-1))*6 + 5] = x+1 + (z+1) * tex->width;
      }
	
  // End of terrain generation
	
  // Create Model and upload to GPU:

  Model* model = LoadDataToModel(
				 vertexArray,
				 normalArray,
				 texCoordArray,
				 NULL,
				 indexArray,
				 vertexCount,
				 triangleCount*3);

  return model;
}

float calcHeight(Model* mod,float x,float z,int texWidth)
{
  int x0,x1,z0,z1;
  float y00,y01,y10,y11,dx0,dz0,yTot;
  x0 = floor(x); 
  x1 = ceil(x); 
  z0 = floor(z); 
  z1 = ceil(z);
  dx0 = x - x0;
  dz0 = z - z0;

  y00 = mod->vertexArray[(x0 + z0 * texWidth)*3 + 1];
  y01 = mod->vertexArray[(x1 + z0 * texWidth)*3 + 1];
  y10 = mod->vertexArray[(x0 + z1 * texWidth)*3 + 1];
  y11 = mod->vertexArray[(x1 + z1 * texWidth)*3 + 1];

  if(dx0 > dz0)
    {
      // Upper triangle
      float dyx = y01 - y00;
      float dyz = y11 - y01;
      yTot = y00 + dyx*dx0 + dyz*dz0*dx0;
      //float tempYz = y01 + dyz*dz0;
    }
  else
    {
      // Lower triangle
      float dyx = y11 - y10;
      float dyz = y10 - y00;
      yTot = y10 + dyx*dx0 + dyz*(1.0-dz0)*dx0;
    }
  return yTot;
}

// vertex array object
Model *tm,*lSource,*sphere1;
// Reference to shader program
GLuint program;
GLuint tex1, tex2;
TextureData ttex; // terrain

void init(void)
{
  // GL inits
  glClearColor(0.2,0.2,0.5,0);
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  printError("GL inits");

  projectionMatrix = frustum(left,right,bottom,top,near,far);

  // Load and compile shader
  program = loadShaders("lab4-4.vert", "lab4-4.frag");
  skyboxProgram = loadShaders("skybox.vert", "skybox.frag");
  printError("init shader");

  glUseProgram(program);
	
  glUniformMatrix4fv(glGetUniformLocation(program, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);
  glUniform1i(glGetUniformLocation(program, "tex"), 0); // Texture unit 0
  LoadTGATextureSimple("textures/grass.tga", &tex1);

  // Load light. THIS IS NOT WORKING YET! CHANGES IN SHADER NEEDED
  glUniform3fv(glGetUniformLocation(program, "lightSource"), 1, lightSource); //&
  glUniform1fv(glGetUniformLocation(program, "specularExponent"), 1, &specularExponent);
	
  // Load terrain data
	
  LoadTGATextureData("textures/fft-terrain.tga", &ttex);
  tm = GenerateTerrain(&ttex);
  printError("init terrain");

  // Key handling
  initKeymapManager();
  vec3 r = SetVector(-0.5,0,-0.5);
  p = SetVector(24,5,24);
  l = VectorAdd(p,r);
  v = SetVector(0,1,0);

  // Place model at light source
  lSource = LoadModelPlus("objects/groundsphere.obj");
  sphere1 = LoadModelPlus("objects/groundsphere.obj");
  mSkybox = LoadModelPlus("objects/skyboxbig.obj");

  //Skybox
  glUseProgram(skyboxProgram);
  
  //skybox.loadImages("textures/skybox/sky%d.tga");
  //skybox.generateCubeMap();
  skybox = Skybox(mSkybox, skyboxProgram);
  skybox.init(projectionMatrix, "textures/skybox/sky%d.tga");

  //printf("BPP for texture[1] after init: %i \n", skybox.texture[1].bpp); // private
  printError("Error: init skybox");
 }

void handleKeyPress()
{
  // --- HANDLE KEY INPUT ----
  if(keyIsDown('w'))
    {
      vec3 w = Normalize(VectorSub(l,p));
      l = VectorAdd(l,ScalarMult(w,constSpeed));
      p = VectorAdd(p,ScalarMult(w,constSpeed));
    }
  if(keyIsDown('s'))
    {
      vec3 s = Normalize(VectorSub(p,l));
      l = VectorAdd(l,ScalarMult(s,constSpeed));
      p = VectorAdd(p,ScalarMult(s,constSpeed));
    }
  if(keyIsDown('a'))
    {
      vec3 w = VectorSub(l,p);
      vec3 a = Normalize(CrossProduct(v,w));
      l = VectorAdd(l,ScalarMult(a,constSpeed));
      p = VectorAdd(p,ScalarMult(a,constSpeed));
    }
  if(keyIsDown('d'))
    {
      vec3 w = VectorSub(l,p);
      vec3 d = Normalize(CrossProduct(w,v));
      l = VectorAdd(l,ScalarMult(d,constSpeed));
      p = VectorAdd(p,ScalarMult(d,constSpeed));
    }
}

void handleMouse(int x,int y)
{
  GLfloat xtemp = (GLfloat)(-x+WIDTH/2)/20000;
  GLfloat ytemp = (GLfloat)(-y+HEIGHT/2)/20000;
  vec3 r = Normalize(VectorSub(l,p)); // Forward Direction
  vec3 d = Normalize(CrossProduct(r,v)); // Right direction
  mat4 phiRot = ArbRotate(v,xtemp);
  r = MultVec3(phiRot,r);
  mat4 thetaRot = ArbRotate(d,ytemp);
  r = MultVec3(thetaRot,r);
  //v = MultVec3(thetaRot,v); // for flight simulator
  l = VectorAdd(p,r);
  //int windowWidth = WIDTH;
  //int windowHeight = HEIGHT;
  //glutWarpPointer(windowWidth,windowHeight);
}

void display(void)
{
  t = (GLfloat)glutGet(GLUT_ELAPSED_TIME) / 3000;
  mat4 total, modelView, camMatrix;
  // Build matrix
  vec3 cam = p; //{0, 5, 8};
  vec3 lookAtPoint = l; //{2, 0, 2};
  vec3 upVector = v;

  camMatrix = lookAtv(cam,lookAtPoint,upVector);
  modelView = IdentityMatrix();
  total = Mult(camMatrix,modelView);

  // clear the screen
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  //Disable Z-buffer before drawing skybox
  glDisable(GL_DEPTH_TEST);
  glUseProgram(skyboxProgram);
  skybox.draw(camMatrix);
  glEnable(GL_DEPTH_TEST);
  printError("Error in display skybox");
	
  printError("pre display");
	
  glUseProgram(program);

  //float test = calcHeight(tm,1,2,3);
  //float test2 = ttex.width;
  //printf("y: %3.2f \n",test2);

  glUniformMatrix4fv(glGetUniformLocation(program, "mdl2World"), 1, GL_TRUE, modelView.m);
  glUniformMatrix4fv(glGetUniformLocation(program, "world2View"), 1, GL_TRUE, camMatrix.m);
  glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, total.m);
  glBindTexture(GL_TEXTURE_2D, tex1);		// Bind Our Texture tex1
  DrawModel(tm, program, "inPosition", "inNormal", "inTexCoord"); // "inNormal"

  // Light source sphere
  modelView = T(50,105,50);
  glUniformMatrix4fv(glGetUniformLocation(program, "mdl2World"), 1, GL_TRUE, modelView.m);
  DrawModel(lSource,program,"inPosition","inNormal","inTexCoord");
  printError("display 1");

  // Sphere1
  float xs,zs,ys;
  int texWidth = ttex.width;
  xs = 70+20*cos(t);
  zs = 70+20*sin(t);
  ys = calcHeight(tm,xs,zs,texWidth);
  modelView = T(xs,ys,zs);
  glUniformMatrix4fv(glGetUniformLocation(program, "mdl2World"), 1, GL_TRUE, modelView.m);
  DrawModel(sphere1,program,"inPosition","inNormal","inTexCoord");
  printError("display 2");
	
  glutSwapBuffers();
}

void timer(int i)
{
  glutTimerFunc(20, &timer, i);
  glutPostRedisplay();
  handleKeyPress();
  glutPassiveMotionFunc(handleMouse);
}

void mouse(int x, int y)
{
  printf("%d %d\n", x, y);
}

int main(int argc, char **argv)
{
  cout << "Funkar nu!" << endl;
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
  glutInitContextVersion(3, 2);
  glutInitWindowSize (WIDTH,HEIGHT);
  glutCreateWindow ("TSBK03 C++");
  glutDisplayFunc(display);
  init();
  glutTimerFunc(20, &timer, 0);

  glutPassiveMotionFunc(mouse);

  glutMainLoop();
  exit(0);
}
