#include "LinearBlender.h"

LinearBlender::LinearBlender(int initOverlap){
  overlap = initOverlap;
}

void LinearBlender::blendCorners(vector<float>* tex00,
				 vector<float>* tex01,
				 vector<float>* tex10,
				 vector<float>* tex11,
				 int width){
  
}

void LinearBlender::blendHors(vector<float>* texWest,vector<float>* texEast,int width){

}

void LinearBlender::blendVert(vector<float>* texNorth,vector<float>* texSouth,int width){
  
}
