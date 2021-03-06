#ifndef _WORLD_
#define _WORLD_

#ifdef __APPLE__
	#include <OpenGL/gl3.h>
	#include "../common/mac/MicroGlut.h"
#elif _WIN32
	#define GLEW_STATIC
	#include <GL/glew.h>
	#include <GL/freeglut.h>
	#include <GL/gl.h>
#elif __linux
	#include <GL/gl.h>
#include "../common/linux/MicroGlut.h"
#endif

#include "Camera.h"
#include "Skybox.h"
#include "TerrainPatch.h"
#include "Blender.h"
#include "Plant.h"
#include "ManageChasersAndEvaders.h"

#include <vector>
#include <algorithm>
#include <thread>
#include <iostream>
#include <time.h>
#include <mutex>

// Debugging includes
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;

class World
{
 private:
  long worldSeed;
  GLfloat time;
  int patchOverlap,patchSize;
  GLuint grassTex1;
  GLuint grassBillboardTex;
  GLuint grassTexLowPass;
  GLuint noiseTex;
  GLuint rockTex1;
  GLuint rockTex2;
  GLuint noiseTex2;
  GLfloat specularExponent;
  float distanceFogConstant;
  void init();
  void printTerrainToFile(int startSize);

 public:
  GLuint phongShader,skyboxShader,terrainShader,grassShader,plantShader, birdShader;
    
  int gridSize;
  
  Camera* camera;
  Skybox* skybox;
#if BIRDS == 1
  ManageChasersAndEvaders* birds;
#endif
  Blender* blender;

  vector<vector<TerrainPatch*>> terrainVector;
  mutex terrainMutex;
  bool updatingWorld;

  World();
  ~World();
  void draw();
  TerrainPatch* generatePatch(int patchX, int patchY);
  void generateStartingPatches(int startSize);
  void addPatchRow(int direction);
  void addTerrainSouth();
  void addTerrainNorth();
  void addTerrainNorth2();
  void addTerrainEast();
  void addTerrainWest();
  void removeTerrainSouth();
  void removeTerrainNorth();
  void removeTerrainEast();
  void removeTerrainWest();
    
  void addGeneratedTerrain();
  void update();
  void updateTerrain();
};

// Threading functions
void threadGeneratePatch(World *w, int x, int y, vector<TerrainPatch*> *terrainRow, mutex * lock, int index);
void threadAddPatchNorth(World *w);
void threadAddPatchSouth(World *w);
void threadAddPatchEast(World *w);
void threadAddPatchWest(World *w);

#endif
