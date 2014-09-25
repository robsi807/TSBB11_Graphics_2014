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

//#include "../common/VectorUtils3.h"
//#include "../common/GL_utilities.h"

class World
{
  private:
  long worldSeed;
  void init();

  public:
    World();
    ~World();
    GLuint phongShader;
    GLuint skyboxShader;
    void draw();
    Camera* camera;
    Skybox* skybox;
    TerrainPatch* terrainPatch;
    TerrainPatch* terrainPatch2;

};

#endif
