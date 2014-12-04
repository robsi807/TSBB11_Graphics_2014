#ifndef _MANAGECHASERSANDEVADERS_
#define _MANAGECHASERSANDEVADERS_

//OpenGL for Linux and Mac OS X included in these headers
#include "../common/VectorUtils3.h"
#include "../common/GL_utilities.h"
#include "../common/loadobj.h"
#include "../common/LoadTGA.h"

#include <cmath>
#include <iostream>
#include <vector>
#include <algorithm>    // std::move (ranges)
#include <utility>      // std::move (objects)

#include <OpenAL/al.h>
#include <OpenAL/alc.h>

#include "Camera.h"
#include "Evader.h"
#include "Chaser.h"

class ManageChasersAndEvaders
{
 private:
  //~ManageChasersAndEvaders();
  void mergeFlocks(Camera *cam);
  void splitFlock(Evader *flock, Camera cam);
  int nearestFlock(Boid chaser);
  void loadEvaderModels();
  void animate(GLfloat time);
  void animateAndDraw(GLfloat time, mat4 cameraMatrix);
  void sortFlockIndex(Camera *cam);
  
  GLuint* shader;
  Model *evaderModel;
  Model *chaserModel;
  vector<Model*> evaderModels;
  GLuint evaderTexture;
  GLuint chaserTexture;

  GLfloat prevTime;
  uint modelIndex;

  bool birdView;
  vec3 camSpeed;

  //FILE *fp;

 public:
  Chaser* chasers;
  vector<Evader*> flocks;
  //vector<Chaser*> predators;

  ManageChasersAndEvaders(GLuint* phongShader, char *modelPathEvader, char *imagePathEvader, char *modelPathChaser, char *imagePathChaser, Camera cam);
  void update(GLfloat time, Camera *cam);  
  void draw(GLfloat time, mat4 cameraMatrix);
};

#endif

