#ifndef _PERLIN_PATCH_GENERATOR_
#define _PERLIN_PATCH_GENERATOR_

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
#include <cmath>
#include <vector>
#include <climits>
#include <iostream>

using namespace std;

class PerlinPatchGenerator : public PatchGenerator{

  private:

    int biotope;
    int NoF;
    int amplitudeScale;
    int gridSize;

	vector<float> addMatrices(vector<float> inGrid1, vector<float> inGrid2);
	float interpolateValues(float a, float b, float x);
	vector<vector<float>> createGradients(int gradientPoints);
	vector<float> createPatch(int frequency, int gradientPoints, float amplitude);
    float dotProduct(vector<float> a,float b[2]);

  public:
    PerlinPatchGenerator(int inputBiotope, int inputNoF, int inputAmplitude, int inputSize);
    void printMatrix(vector<float> matrix);
    vector<float> generatePatch(int xPatch, int yPatch);

};

#endif
