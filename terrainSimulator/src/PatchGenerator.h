#ifndef _PATCH_GENERATOR_
#define _PATCH_GENERATOR_

#include "../common/LoadTGA.h"

class PatchGenerator{

  public:
    virtual TextureData generatePatch(int size) = 0;
};



#endif
