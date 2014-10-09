#ifndef _TERRAINPATCH_
#define _TERRAINPATCH_

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

#include<vector>
using namespace std;

class TerrainPatch
{
  
  private:
    int posX, posY;
    int patchWidth, patchHeight;
    void generateGeometry();
    GLuint* shader;

  public:
    TerrainPatch(vector<float> tex,int width, int height, int x, int y, GLuint* phongShader, char *imagePath);
    vec3 calcNormal(vec3 v0, vec3 v1, vec3 v2);
    float calcHeight(float x,float z,int texWidth);
    Model* geometry;
    ~TerrainPatch();
    vector<float> rawHeightMap;
    vector<float> blendedHeightMap;
    GLuint texture;
    void draw(mat4 cameraMatrix);
};


#endif
