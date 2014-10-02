#ifndef _LINEAR_BLENDER_
#define _LINEAR_BLENDER_

#include "../common/LoadTGA.h"
#include "Blender.h"

class LinearBlender : public Blender{

 public:
  LinearBlender(int initOverlap);

  void blendCorners(TextureData* tex00,
		    TextureData* tex01,
		    TextureData* tex10,
		    TextureData* tex11);

  void blendX(TextureData* texLeft,TextureData* texRight);
  void blendY(TextureData* texTop,TextureData* texDown);
};



#endif
