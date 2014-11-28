#ifndef _WORLD_OBJECT_
#define _WORLD_OBJECT_

#include "Camera.h"
#include "loadobj.h"
#include "VectorUtils3.h"

using namespace std;

class WorldObject{

  public:
    vec3 position;
    virtual void draw(mat4 cameraMatrix, float time) = 0;
};



#endif
