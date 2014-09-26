#include "MockupPatchGenerator.h"

MockupPatchGenerator::MockupPatchGenerator(char* imagePath){
  LoadTGATextureData(imagePath, &heightMap);
}

TextureData MockupPatchGenerator::generatePatch(int size){

  return heightMap;
}
