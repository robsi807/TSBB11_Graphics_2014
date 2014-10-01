#ifndef _MOCKUP_PATCH_GENERATOR_
#define _MOCKUP_PATCH_GENERATOR_

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

#include "PatchGenerator.h"

class MockupPatchGenerator : public PatchGenerator{

  private:
    TextureData heightMap;

  public:

    MockupPatchGenerator(char* imagePath);

    TextureData generatePatch(int size);

};

#endif
