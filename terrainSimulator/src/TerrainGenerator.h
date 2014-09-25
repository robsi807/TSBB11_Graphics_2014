#ifndef _TERRAIN_GENERATOR
#define _TERRAIN_GENERATOR_

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

#include "../common/LoadTGA.h"
#include "../common/loadobj.h"
#include "../common/VectorUtils3.h"

class TerrainGenerator
{
  private:
    long worldSeed;

  public:
    TerrainGenerator();
    TerrainGenerator(long seed);
    vec3 calcNormal(vec3 v0, vec3 v1, vec3 v2);
    Model* generateTerrain(TextureData *tex);
    float calcHeight(Model* mod,float x,float z,int texWidth);

};


#endif
