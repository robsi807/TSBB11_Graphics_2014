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
#include "Camera.h"

#include <math.h>
#include <iostream>

using namespace std;

#define PI 3.1415

#define WIDTH 1024
#define HEIGHT 860

// Skybox
Skybox skybox;
GLuint skyboxProgram;
Model *mSkybox;

Camera cam;

GLfloat t = 0;

//mat4 projectionMatrix;

const GLfloat lightSource[3] = {50.0f, 100.0f, 50.0f}; //Point3D
GLfloat specularExponent = 50;

vec3 calcNormal(vec3 v0, vec3 v1, vec3 v2)
{
  vec3 n = CrossProduct(VectorSub(v1,v0),VectorSub(v2,v0));
  if(n.y < 0)
    n = ScalarMult(n,-1);
  return Normalize(n);
}

Model* generateTerrain(TextureData *tex)
{
  int vertexCount = tex->width * tex->height;
  int triangleCount = (tex->width-1) * (tex->height-1) * 2;
  int x, z;

  GLfloat vertexArray[3 * vertexCount];
  GLfloat normalArray[3 * vertexCount];
  GLfloat texCoordArray[ 2 * vertexCount];
  GLuint indexArray[3 * triangleCount];

  float hScale = 20.0;

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

        float y10 = tex->imageData[((int)x0 + (int)z1 * tex->width) * (tex->bpp/8)] / hScale;
        float y11 = tex->imageData[((int)x1 + (int)z1 * tex->width) * (tex->bpp/8)] / hScale;
        float y12 = tex->imageData[((int)x2 + (int)z1 * tex->width) * (tex->bpp/8)] / hScale;

        float y21 = tex->imageData[((int)x1 + (int)z2 * tex->width) * (tex->bpp/8)] / hScale;
        float y22 = tex->imageData[((int)x2 + (int)z2 * tex->width) * (tex->bpp/8)] / hScale;

        vec3 p00 = {x0,y00,z0};
        vec3 p01 = {x1,y01,z0};

        vec3 p10 = {x0,y10,z1};
        vec3 p11 = {x1,y11,z1};
        vec3 p12 = {x2,y12,z1};

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
      }

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

  //projectionMatrix = frustum(left,right,bottom,top,near,far);

  // Init camera
  cam.init(vec3(24,20,24), WIDTH, HEIGHT, 0.7, 7);

  // Load and compile shader
  program = loadShaders("lab4-4.vert", "lab4-4.frag");
  skyboxProgram = loadShaders("skybox.vert", "skybox.frag");
  printError("init shader");

  glUseProgram(program);

  glUniformMatrix4fv(glGetUniformLocation(program, "projMatrix"), 1, GL_TRUE, cam.projectionMatrix.m);
  glUniform1i(glGetUniformLocation(program, "tex"), 0); // Texture unit 0
  LoadTGATextureSimple("textures/grass.tga", &tex1);

  // Load light. THIS IS NOT WORKING YET! CHANGES IN SHADER NEEDED
  glUniform3fv(glGetUniformLocation(program, "lightSource"), 1, lightSource); //&
  glUniform1fv(glGetUniformLocation(program, "specularExponent"), 1, &specularExponent);

  // Load terrain data

  LoadTGATextureData("textures/fft-terrain.tga", &ttex);
  tm = generateTerrain(&ttex);
  printError("init terrain");

  // Place model at light source
  lSource = LoadModelPlus("objects/groundsphere.obj");
  sphere1 = LoadModelPlus("objects/groundsphere.obj");
  mSkybox = LoadModelPlus("objects/skyboxbig.obj");

  //Skybox
  glUseProgram(skyboxProgram);

  //skybox.loadImages("textures/skybox/sky%d.tga");
  //skybox.generateCubeMap();
  skybox = Skybox(mSkybox, skyboxProgram);
  skybox.init(cam.projectionMatrix, "textures/skybox/sky%d.tga");

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

  glUseProgram(program);
  glUniformMatrix4fv(glGetUniformLocation(program, "mdl2World"), 1, GL_TRUE, modelView.m);
  glUniformMatrix4fv(glGetUniformLocation(program, "world2View"), 1, GL_TRUE, cam.cameraMatrix.m);
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
  glutInitWindowSize (WIDTH,HEIGHT);
  glutCreateWindow ("TSBK03 C++");
  glutDisplayFunc(display);
  init();
  glutTimerFunc(20, &timer, 0);

  glutPassiveMotionFunc(mouse);

  glutMainLoop();
  exit(0);
}
