#ifndef _LINEAR_BLENDER_
#define _LINEAR_BLENDER_

#include "../common/LoadTGA.h"
#include "Blender.h"

class LinearBlender : public Blender{

 public:
  LinearBlender(int initOverlap);

  void blendCorners(vector<float>* tex00,
		    vector<float>* tex01,
		    vector<float>* tex10,
		    vector<float>* tex11);

  void blendHors(vector<float>* texWest,vector<float>* texEast);
  void blendVert(vector<float>* texNorth,vector<float>* texSouth);
};



#endif
