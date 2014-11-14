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
#include "PatchGenerator.h"
#include "PerlinPatchGenerator.h"
#include "ValuePatchGenerator.h"
#include "Skybox.h"
#include "TerrainPatch.h"
#include "LinearBlender.h"

#include <vector>
#include <algorithm>
#include <thread>
#include <iostream>
#include <time.h>

//#include "../common/VectorUtils3.h"
//#include "../common/GL_utilities.h"

// Patch specific defines
#define PATCH_OVERLAP 64
#define PATCH_SIZE 512
#define GRID_BEGIN_SIZE 3

// Direction specific defines
#define NORTH 8
#define SOUTH 2
#define EAST 6
#define WEST 4

class World
{
  private:
    long worldSeed;
    GLfloat time;
    int patchOverlap,patchSize,gridSize;
    void init();
    void drawTerrainVector(TerrainPatch* t);
    

  public:
    Model* sphere;
    
    GLuint phongShader,skyboxShader,terrainShader;
    GLuint terrainTexture;
    Camera* camera;
    Skybox* skybox;
    PatchGenerator* patchGenerator;
    Blender* blender;
    std::vector<vector<TerrainPatch*>> terrainVector;
    std::vector<TerrainPatch*> generatedTerrain;

    World();
    ~World();
    void draw();
    TerrainPatch* generatePatch(int patchX, int patchY);
    void generateStartingPatches(int startSize);
    void addPatchRow(int direction);
    void addGeneratedTerrain();
    void update();
    void updateTerrain(vec3 position, vec3 direction);

};

#endif
