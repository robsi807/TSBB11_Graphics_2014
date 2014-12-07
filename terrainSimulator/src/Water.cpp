#include "Water.h"


void generateTextureMap(vector<float> waterMap, int mapSize);

Water::Water(vector<float> initHeightMap, int mapSize)
{
  generateWaterMap(initHeightMap, mapSize);
  generateTextureMap(waterMap, mapSize);
}

Water::Water(int x, int y, GLuint *shader)
{
  
  xPos = x;
  yPos = y;
  waterShader = shader;
}

Water::Water() {

}

void Water::generateWater(vector<float> blendedHeightMap, int _mapSize, float _heightScale) {

  mapSize = _mapSize;
  
  heightScale = _heightScale;

  waterHeight = WATER_LEVEL / heightScale;

  generateWaterMap(blendedHeightMap, mapSize);
  generateTextureMap(waterMap, mapSize);
  generateWaterModel(heightScale);
  
}

void Water::generateWaterMap(vector<float> initHeightMap, int mapSize) {

  const int overlapSize = 32; // temp solution.
  
  waterMap.resize(initHeightMap.size(), 0.0);
  //mountainMap.resize(initHeightMap.size(), 0.0);
  
  float waterMax = 0.0;
  
  for(int i = 0; i < initHeightMap.size();i = i+1) {
    if(initHeightMap.at(i) > waterMax) {
      waterMax = initHeightMap.at(i);
    }
    if(initHeightMap.at(i) < WATER_LEVEL) {
      //cout << "WE GOT WATER LOWER THAN " << WATER_LEVEL << "\n";
      //cout << "The level is:  " << initHeightMap.at(i) << "\n";
      waterMap.at(i) = 0.5;
    }
    
    if(initHeightMap.at(i) > MOUNTAIN_LEVEL) {
      
      waterMap.at(i) = 1.0;
      highPointIndexVector.push_back(i);
    }
  }
  
  for (int i = 0; i < NUMBER_OF_WATER_SOURCES;i++) {
    cout << highPointIndexVector.size() << " is our high point index vector size! \n";
    waterSources.push_back(highPointIndexVector.at((int)rand() % highPointIndexVector.size()));
  }
  
  if(waterSources.size() > 0) {
    WaterCube cube;
    int riverSize = 100;
    
    int source = waterSources.at(0);
    cout << "our river starts at " << initHeightMap.at(source) << "\n";
    int x = source % mapSize;
    int y = initHeightMap.at(source) / heightScale;
    int z = source / mapSize;
    
    cout << "our river's actual position (" << x << "," << y << "," << z << ")\n";
    
    
    for (int i = 0 ; i < riverSize; i++) {
        source = 0;
        x = (source + i*5) % mapSize;
        y = initHeightMap.at(source + i*5) / heightScale;
        z = source / mapSize;
        cube.xPos = x;
        cube.yPos = y;
        cube.zPos = z;
        cube.side = CUBE_SIZE;
        cube.height = 4;
        
        riverModel.push_back(cube);
    
    }
  }
  
  
  cout << "generated water map! Highest point was: " << waterMax << "! \n";
  cout << "we got " << highPointIndexVector.size() << " high points! \n";
  
  
}

  void generateTextureMap(vector<float> waterMap, int mapSize) {
  
  
  
  
  
  
  
  GLuint texId = (GLuint) 10;
  
  glGenTextures(1, &texId);			// Generate OpenGL texture IDs
	glBindTexture(GL_TEXTURE_2D, texId);		// Bind Our Texture
  
  //float* waterMapData = &waterMap[0];
  
  
  int size = sqrt(waterMap.size());
  
  
  
  unsigned data[size*size];
  for (int y = 0; y < size; y++) {
      for (int x = 0; x < size; x++) {
          if (waterMap.at(size*y + x) == 1.0) {//((x - size/2)*(x - size/2) + (y - size/2)*(y - size/2) < size) {
              data[size*y + x] = (0xFF << 16) ;//((156 << 24) | (256 << 16) | (156 << 8) | (200 << 0));
          } else if (waterMap.at(size*y + x) == 0.5) { 
              data[size*y + x] = (0xFF << 24);
          
          } else {
              //cout << "Y is now: " << y << "\n";
              //cout << "X is now: " << y << "\n";
              data[size*y + x] = (0xA0 << 8);  // I'd expect this to be transparent and the above to be slightly transparent and green, but it's red somehow.
          }
          
      }
  }

  glBindTexture(GL_TEXTURE_2D, texId);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);  //Always set the base and max mipmap levels of a texture.
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, size, size, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, (GLvoid*)data);
  
  

  

  //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mapSize, mapSize, 0, GL_RGB, GL_FLOAT, &waterMap.front());
  
  
}

void Water::generateWaterModel(float heightScale){

    int blendedWidth = 2;
    int blendedHeight = 2;
    //int offset = patchOverlap/2;

    int vertexCount = 4;
    int triangleCount = 2;

    int x, z;

    GLfloat *vertexArray = (GLfloat *)malloc(sizeof(GLfloat) * 3 * vertexCount);
    GLfloat *normalArray = (GLfloat *)malloc(sizeof(GLfloat) * 3 * vertexCount);
    GLfloat *texCoordArray = (GLfloat *)malloc(sizeof(GLfloat) * 2 * vertexCount);
    GLuint *indexArray = (GLuint *)malloc(sizeof(GLuint) * triangleCount*3);
    // GLfloat vertexArray[3 * vertexCount];
    // GLfloat normalArray[3 * vertexCount];
    // GLfloat texCoordArray[ 2 * vertexCount];
    // GLuint indexArray[3 * triangleCount];

    //float heightScale = 0.0025;


    for (x = 0; x < blendedWidth; x++) {
        for (z = 0; z < blendedHeight; z++)
	      {
            cout << "Generating a water vertice!\n";
            vertexArray[(x + z * blendedWidth)*3 + 0] = x * mapSize / 1.0;
	          vertexArray[(x + z * blendedWidth)*3 + 1] = z * mapSize/ 1.0;
	          vertexArray[(x + z * blendedWidth)*3 + 2] = (WATER_LEVEL) / heightScale;
	          
	          
	          
	          // Normal vectors. Water, default is up!
            normalArray[(x + z * blendedWidth)*3 + 0] = 0.0; //(y1-y0)*(z2-z0)-(y2-y0)*(z1-z0); //0.0;
            normalArray[(x + z * blendedWidth)*3 + 1] = 0.0; //(z1-z0)*(x2-x0)-(z2-z0)*(x1-x0); //1.0;
            normalArray[(x + z * blendedWidth)*3 + 2] = 1.0; //(x1-x0)*(y2-y0)-(x2-x0)*(y1-y0); //0.0;

            // Texture coordinates. You may want to scale them.
            texCoordArray[(x + z * blendedWidth)*2 + 0] = ((float)x / mapSize); // (float)x / size;
            texCoordArray[(x + z * blendedWidth)*2 + 1] = ((float)z / mapSize); // (float)z / patchHeight;
        }
    }
    
    for (x = 0; x < blendedWidth-1; x++) {
        for (z = 0; z < blendedHeight-1; z++)
        {
            // Triangle 1
            indexArray[(x + z * (blendedWidth-1))*6 + 0] = 0;//x + z * 2;
            indexArray[(x + z * (blendedWidth-1))*6 + 1] = 2, //x + (z+1) * 2;
            indexArray[(x + z * (blendedWidth-1))*6 + 2] = 1, //x+1 + z * 2;
            // Triangle 2
            indexArray[(x + z * (blendedWidth-1))*6 + 3] = 1;   //x+1 + z * 2;
            indexArray[(x + z * (blendedWidth-1))*6 + 4] = 2;   //x + (z+1) * 2;
            indexArray[(x + z * (blendedWidth-1))*6 + 5] = 3;   //x+1 + (z+1) * 2;
    
        }
    }
    
    // Triangle 1
    indexArray[0] = 0;//x + z * 2;
    indexArray[1] = 1, //x + (z+1) * 2;
    indexArray[2] = 2, //x+1 + z * 2;
    // Triangle 2
    indexArray[3] = 2;   //x+1 + z * 2;
    indexArray[4] = 1;   //x + (z+1) * 2;
    indexArray[5] = 3;   //x+1 + (z+1) * 2;
        
  	surface = (Model*) malloc(sizeof(Model));
    memset(surface, 0, sizeof(Model));

    surface->vertexArray = vertexArray;
    surface->texCoordArray = texCoordArray;
    surface->normalArray = normalArray;
    surface->indexArray = indexArray;
    surface->numVertices = vertexCount;
    surface->numIndices = triangleCount*3;

    surface = LoadModelPlus("../objects/cubeplus.obj");


}

void Water::uploadGeometry() {
  cout << "TerrainPatch::uploadGeometry: uploading geometry!\n";
  
  //water.uploadGeometry();
  
	BuildModelVAO2(surface);
	
  cout << "TerrainPatch::uploadGeometry: geometry DONE uploading! <3\n";
}


void Water::draw(mat4 cameraMatrix,float time) {

    int offset = 64;
    mat4 modelView = Mult(T(xPos+(mapSize+offset)/2,waterHeight, yPos+(mapSize+offset)/2), S(mapSize,0,mapSize));
    // Draw terrain normally
    glUseProgram(*waterShader);
    glUniformMatrix4fv(glGetUniformLocation(*waterShader, "mdl2World"), 1, GL_TRUE, modelView.m);
    glUniformMatrix4fv(glGetUniformLocation(*waterShader, "world2View"), 1, GL_TRUE, cameraMatrix.m);
    DrawModel(surface, *waterShader, "inPosition", "inNormal","inTexCoord"); 
    
    for(int i = 0; i < riverModel.size();i++) {
        WaterCube cube = riverModel.at(i);
        mat4 modelView = Mult(T(xPos+offset/2 + cube.zPos,cube.yPos, yPos+offset/2+cube.xPos), S(cube.side,cube.height,cube.side));
        // Draw terrain normally
        glUseProgram(*waterShader);
        glUniformMatrix4fv(glGetUniformLocation(*waterShader, "mdl2World"), 1, GL_TRUE, modelView.m);
        glUniformMatrix4fv(glGetUniformLocation(*waterShader, "world2View"), 1, GL_TRUE, cameraMatrix.m);
        DrawModel(surface, *waterShader, "inPosition", "inNormal","inTexCoord"); 
    }
    

  
}



Water::~Water()
{

}
