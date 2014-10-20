#ifndef _PATCH_GENERATOR_
#define _PATCH_GENERATOR_


#include "LoadTGA.h"

#include <vector>
using namespace std;

class PatchGenerator{

  public:
    virtual vector<float> generatePatch(int xPatch, int yPatch, int size) = 0;
    virtual void printMatrix(vector<float> matrix, int matrixSize) = 0; // shouldn't be here, probably....... >^^<"
};



#endif
