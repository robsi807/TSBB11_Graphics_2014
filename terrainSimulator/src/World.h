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
#include "Skybox.h"
#include "TerrainPatch.h"

#include <vector>
#include <algorithm>
#include <thread>
#include <iostream>
#include <time.h>

//#include "../common/VectorUtils3.h"
//#include "../common/GL_utilities.h"

class World
{
  private:
    long worldSeed;

		GLfloat time;

  public:
    GLuint phongShader;
    GLuint skyboxShader;
    Camera* camera;
    Skybox* skybox;
    PatchGenerator* patchGenerator;
    std::vector<TerrainPatch*> terrainVector;
    std::vector<TerrainPatch*> generatedTerrain;

    World();
    ~World();
    void draw();
    void generatePatch(int patchX, int patchY, int patchSize);
    void addGeneratedTerrain();
    void update();
		void updateTerrain(vec3 position, vec3 direction);

};

#endif
