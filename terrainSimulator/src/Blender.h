#ifndef _BLENDER_
#define _BLENDER_

#include "../common/LoadTGA.h"

class Blender{

 protected:
  int overlap;

 public:
  virtual void blendCorners(TextureData* tex00,
			    TextureData* tex01,
			    TextureData* tex10,
			    TextureData* tex11) = 0;

  virtual void blendX(TextureData* texLeft,TextureData* texRight) = 0;
  virtual void blendY(TextureData* texTop,TextureData* texDown) = 0;
};

#endif
