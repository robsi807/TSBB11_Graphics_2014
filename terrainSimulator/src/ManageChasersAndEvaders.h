//____________________________ManageChasersAndEvaders.cpp____________________________
// Description: Header file for handling chasers (predators) and evaders (flocks).
// Author: Carl Stejmar, carst056@student.liu.se
// Date: 2014-12-10
//___________________________________________________________________________________

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

#include "Camera.h"
#include "Evader.h"
#include "Chaser.h"

class ManageChasersAndEvaders
{
 private:
  void mergeFlocks(Camera *cam);
  void splitFlock(Evader *flock, Camera cam);
  int nearestFlock(Boid chaser);
  void loadEvaderModels();
  //void animate(GLfloat time);
  void animateAndDraw(GLfloat time, Camera cam);
  void drawChasers(Camera cam);
  void sortFlockIndex(Camera *cam);
  void followCam(Camera *cam);

  Chaser* chasers;
  vector<Evader*> flocks;
  //vector<Chaser*> predators;
  
  GLuint* shader;
  Model *chaserModel;
  vector<Model*> evaderModels;
  GLuint evaderTexture;
  GLuint chaserTexture;

  GLfloat prevTime;
  uint modelIndex;

 public:
  ManageChasersAndEvaders(GLuint* phongShader, char *modelPathEvader, char *imagePathEvader, char *modelPathChaser, char *imagePathChaser, Camera cam);
   ~ManageChasersAndEvaders();
  void printInfo();
  void update(GLfloat time, Camera *cam);  
  void draw(GLfloat time, Camera cam);
};

#endif

