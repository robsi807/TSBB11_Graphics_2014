#include "TerrainPatch.h"


//Constructor
TerrainPatch::TerrainPatch(int patchSize, int x, int y, int overlap, GLuint* terrainShade, GLuint *grassShade) : xGrid(x), yGrid(y), size(patchSize), patchOverlap(overlap){ 

  //blendedSize is +1 to have one more vertex than size
  blendedSize = patchSize-overlap+1;
 
  // Calculate positions
  xPos = x * (patchSize - overlap);
  yPos = y * (patchSize - overlap);

  terrainShader = terrainShade;
  grassShader = grassShade;
  
  //generateGeometry();
  geometry = NULL;
  geometryBoolean = false;

  heightScale = patchSize;
  
  int biotope = 1; // 1 = mountains, 2 = desert
#if PATCH_SIZE == 256
  int NoF = 8;
#else
  int NoF = 9; // Number of frequencies, 1 <= NoF <= 9. Standard = 9. Max value on n: 2^n <= size
#endif
  int amplitude = 1; //Scalefactor for the amplitude. Standard = 1.  
  
  int n=x+y*57;
  n=(n<<13)^n;
  seed=(n*(n*n*60493+19990303)+1376312589)&0x7fffffff;
  rng.seed(seed);
#if PERLIN==1
  patchGenerator = new PerlinPatchGeneratorFast(biotope, NoF, amplitude, patchSize,seed);
#else
    patchGenerator = new ValuePatchGenerator(biotope, NoF, amplitude, patchSize,seed);
#endif
  rawHeightMap = patchGenerator->generatePatch(x,y);
  blendedHeightMap = rawHeightMap;

}
//Calculates the normal of
vec3 TerrainPatch::calcNormal(vec3 v0, vec3 v1, vec3 v2)
{
  vec3 n = CrossProduct(VectorSub(v1,v0),VectorSub(v2,v0));
  if(n.y < 0)
    n = ScalarMult(n,-1);
  return Normalize(n);
}

//Generates the full model from its heightmap
void TerrainPatch::generateGeometry(){
  if(!hasGeometry()){
    int blendedWidth = size-patchOverlap+1;
    int blendedHeight = size-patchOverlap+1;
    int offset = patchOverlap/2;

    int vertexCount = blendedWidth * blendedHeight;
    int triangleCount = (blendedWidth-1) * (blendedHeight-1) * 2;

    int x, z;

    //Allocate memory for all arrays
    GLfloat *vertexArray = (GLfloat *)malloc(sizeof(GLfloat) * 3 * vertexCount);
    GLfloat *normalArray = (GLfloat *)malloc(sizeof(GLfloat) * 3 * vertexCount);
    GLfloat *texCoordArray = (GLfloat *)malloc(sizeof(GLfloat) * 2 * vertexCount);
    GLuint *indexArray = (GLuint *)malloc(sizeof(GLuint) * triangleCount*3);
    
    for (x = 0; x < blendedWidth; x++)
      for (z = 0; z < blendedHeight; z++)
	  {
	    vec3 n = vec3(0.0,0.0,0.0);

	    int x0 = (x+offset-1);
	    int z0 = (z+offset-1);

	    int x1 = x+offset;
	    int z1 = z+offset;

	    int x2 = (x+offset+1);
	    int z2 = (z+offset+1);

	    float y00 = blendedHeightMap.at((x0 + z0 * size)) * heightScale;
	    float y01 = blendedHeightMap.at((x1 + z0 * size)) * heightScale;

	    float y10 = blendedHeightMap.at((x0 + z1 * size)) * heightScale;
	    float y11 = blendedHeightMap.at((x1 + z1 * size)) * heightScale;
	    float y12 = blendedHeightMap.at((x2 + z1 * size)) * heightScale;

	    float y21 = blendedHeightMap.at((x1 + z2 * size)) * heightScale;
	    float y22 = blendedHeightMap.at((x2 + z2 * size)) * heightScale;

	    vec3 p00 = vec3(x0,y00,z0);
	    vec3 p01 = vec3(x1,y01,z0);

	    vec3 p10 = vec3(x0,y10,z1);
	    vec3 p11 = vec3(x1,y11,z1);
	    vec3 p12 = vec3(x2,y12,z1);

	    vec3 p21 = vec3(x1,y21,z2);
	    vec3 p22 = vec3(x2,y22,z2);

	    vec3 n0 = calcNormal(p11,p10,p00);
	    vec3 n1 = calcNormal(p11,p00,p01);
	    vec3 n2 = calcNormal(p11,p01,p12);
	    vec3 n3 = calcNormal(p11,p12,p22);
	    vec3 n4 = calcNormal(p11,p21,p22);
	    vec3 n5 = calcNormal(p11,p10,p21);

	    n = VectorAdd(n0,n1);
	    n = VectorAdd(n,n2);
	    n = VectorAdd(n,n3);
	    n = VectorAdd(n,n4);
	    n = VectorAdd(n,n5);
	    n = Normalize(n);

	    vertexArray[(x + z * blendedWidth)*3 + 0] = x1 - offset;
	    vertexArray[(x + z * blendedWidth)*3 + 1] = blendedHeightMap.at((x1 + z1 * size)) * heightScale;
	    vertexArray[(x + z * blendedWidth)*3 + 2] = z1 - offset;

	    normalArray[(x + z * blendedWidth)*3 + 0] = n.x; //(y1-y0)*(z2-z0)-(y2-y0)*(z1-z0); //0.0;
	    normalArray[(x + z * blendedWidth)*3 + 1] = n.y; //(z1-z0)*(x2-x0)-(z2-z0)*(x1-x0); //1.0;
	    normalArray[(x + z * blendedWidth)*3 + 2] = n.z; //(x1-x0)*(y2-y0)-(x2-x0)*(y1-y0); //0.0;

	    texCoordArray[(x + z * blendedWidth)*2 + 0] = ((float)x1 - offset); // (float)x / size;
	    texCoordArray[(x + z * blendedWidth)*2 + 1] = ((float)z1 - offset); // (float)z / patchHeight;
	  }
    for (x = 0; x < blendedWidth-1; x++)
      for (z = 0; z < blendedHeight-1; z++)
	    {
	      // Triangle 1
	      indexArray[(x + z * (blendedWidth-1))*6 + 0] = x + z * blendedWidth;
	      indexArray[(x + z * (blendedWidth-1))*6 + 1] = x + (z+1) * blendedWidth;
	      indexArray[(x + z * (blendedWidth-1))*6 + 2] = x+1 + z * blendedWidth;
	      // Triangle 2
	      indexArray[(x + z * (blendedWidth-1))*6 + 3] = x+1 + z * blendedWidth;
	      indexArray[(x + z * (blendedWidth-1))*6 + 4] = x + (z+1) * blendedWidth;
	      indexArray[(x + z * (blendedWidth-1))*6 + 5] = x+1 + (z+1) * blendedWidth;
	    }

           
	/*	
    geometry = LoadDataToModel(
			       vertexArray,
			       normalArray,
			       texCoordArray,
			       NULL,
			       indexArray,
			       vertexCount,
			       triangleCount*3);
			       
			       
	*/
				       
	geometry = (Model*) malloc(sizeof(Model));
	memset(geometry, 0, sizeof(Model));
	
	geometry->vertexArray = vertexArray;
	geometry->texCoordArray = texCoordArray;
	geometry->normalArray = normalArray;
	geometry->indexArray = indexArray;
	geometry->numVertices = vertexCount;
	geometry->numIndices = triangleCount*3;
	
	// move away when threading...
	//uploadGeometry();   
	
  // Add plants
#if PLANTS == 1
  addPlants();
#endif
  }
  
}

// Checks so the plants postion is OK
bool TerrainPatch::checkPlantPosition(float xPos,float zPos){
  // Make sure the plant is inside the patch
  if(xPos < patchOverlap/2 || xPos > blendedSize - patchOverlap/2 
    || zPos < patchOverlap/2 || zPos > blendedSize - patchOverlap/2){
    return false;
  }
  
  // Make sure the plant is not on a steep hill
  vec3 normal0 = calcNormalSimple(xPos,zPos);
  float slope = 1.2-normal0.y;
  if(slope > 0.5){
    return false;
  }
      
  return true;
}

void TerrainPatch::addPlants(){  
  //Position based seed
  boost::uniform_int<> one_to_six( 0, INT_MAX );
  boost::variate_generator<RNGType, boost::uniform_int<>> dice(rng, one_to_six);
  
  int bushPatches = 1;
  for(int i=0;i<bushPatches;i++){
    // Add bushes
    // Randomize a start position
    int xMid = patchOverlap/2 + dice() % (blendedSize-patchOverlap/2);  
    int zMid = patchOverlap/2 + dice() % (blendedSize-patchOverlap/2);  
    
    // Randomize a grid size between 3 and 5
    int xGridSize = 25 + dice() % 10;
    int zGridSize = 25 + dice() % 10;
    float gridOffset = 10.0 + ((float)((dice()%300) - 150))/100.0;
   // printf("xGridSize=%i, zGridSize=%i, gridOffset=%i\n",xGridSize,zGridSize,gridOffset);
    
    for(int x = 0; x < xGridSize; x++){
      for(int z = 0; z < zGridSize; z++){
        // Randomize offset from middle position
        float newXPos = ((float)xMid) + x*gridOffset + (dice()%(2*(int)gridOffset-1) - gridOffset/2);
        float newZPos = ((float)zMid) + z*gridOffset + (dice()%(2*(int)gridOffset-1) - gridOffset/2);
	        // Place plant if position is OK
        if(checkPlantPosition(newXPos,newZPos)){
	        float newYPos = calcHeight(newXPos,newZPos);
	        vec3 newPos = vec3(newXPos,newYPos,newZPos);
      
      		// Randomly select if plant is to be placed
      		bool placePlant = (dice() % 10000) < 10000*0.2;
      		if(placePlant){
          	float scale = 1.5 + ((float)(dice() % 150)) /100.0;
          	float angle = 3.1415*((float)(dice() % 200))/100.0;
          	WorldObject* plant = new Plant(newPos,angle,scale,vec3(xPos,0.0,yPos),Bush);
          	objects.push_back(plant);
        	}
        }
        else{
          //printf("Position not allowed: %f, %f !\n",newXPos,newYPos);
        }
      }
    }
  }

  // Add trees
  int treePatches = 1;  
  for(int i=0;i<treePatches;i++){  
    // Randomize a start position
    int xMid = patchOverlap/2 + dice() % (blendedSize-patchOverlap/2);  
    int zMid = patchOverlap/2 + dice() % (blendedSize-patchOverlap/2);  
    
    // Randomize a grid size between 3 and 5
    int xGridSize = 20 + dice() % 3;
    int zGridSize = 20 + dice() % 3;
    float gridOffset = 22.0 + ((float)((dice()%800) - 400))/100.0;
   // printf("xGridSize=%i, zGridSize=%i, gridOffset=%i\n",xGridSize,zGridSize,gridOffset);
    
    for(int x = 0; x < xGridSize; x++){
      for(int z = 0; z < zGridSize; z++){
        // Randomize offset from middle position
        float newXPos = ((float)xMid) + x*gridOffset + (dice()%8 - 4);
        float newZPos = ((float)zMid) + z*gridOffset + (dice()%8 - 4);
	        // Place plant if position is OK
        if(checkPlantPosition(newXPos,newZPos)){
	        float newYPos = calcHeight(newXPos,newZPos);
	        vec3 newPos = vec3(newXPos,newYPos,newZPos);
      
          float scale = 10.5 + ((float)(dice() % 250)) /100.0;
          float angle = 3.1415*((float)(dice() % 200))/100.0;
          //WorldObject* plant = new Plant(newPos,angle,scale,vec3(xPos,0.0,yPos),Tree);
          //objects.push_back(plant);
        }
        else{
          //printf("Position not allowed: %f, %f !\n",newXPos,newYPos);
        }
      }
    }
  }
}

void TerrainPatch::uploadGeometry() {
  
	BuildModelVAO2(geometry);
	
  geometryBoolean = true;
}

void TerrainPatch::generateAndUploadGeometry() {
  generateGeometry();
  uploadGeometry();
}

vec3 TerrainPatch::calcNormalSimple(float xPos, float zPos)
{

  int x,z;
  int offset = patchOverlap/2;
  vec3 normalVec;
  x = (int) floor(xPos);
  z = (int) floor(zPos);
  normalVec.x = geometry->normalArray[(x + z * blendedSize)*3 + 0];
  normalVec.y = geometry->normalArray[(x + z * blendedSize)*3 + 1];
  normalVec.z = geometry->normalArray[(x + z * blendedSize)*3 + 2];
  return normalVec;
}

// Only takes integer positions
float TerrainPatch::calcHeightSimple(int xPos,int zPos){
  int offset = patchOverlap/2;
  int x = xPos - offset;
  int z = zPos - offset; 
  return geometry->vertexArray[(x + z * blendedSize)*3 + 1];
}

float TerrainPatch::calcHeight(float xPos,float zPos)
{  
  float offset = ((float)patchOverlap)/2.0;
  float x = xPos;
  float z = zPos;
  int x0,x1,z0,z1;
  float y00,y01,y10,y11,dx0,dz0,yTot;
  x0 = floor(x); 
  x1 = ceil(x); 
  z0 = floor(z); 
  z1 = ceil(z);
  dx0 = x - x0;
  dz0 = z - z0;

  y00 = geometry->vertexArray[(x0 + z0 * blendedSize)*3 + 1];
  y01 = geometry->vertexArray[(x1 + z0 * blendedSize)*3 + 1];
  y10 = geometry->vertexArray[(x0 + z1 * blendedSize)*3 + 1];
  y11 = geometry->vertexArray[(x1 + z1 * blendedSize)*3 + 1];
  if(dx0 > dz0)
    {
      // Upper triangle
      float dyx = y01 - y00;
      float dyz = y11 - y01;
      yTot = y00 + dyx*dx0 + dyz*dz0*dx0;
      //float tempYz = y01 + dyz*dz0;
    }
  else
    {
      // Lower triangle
      float dyx = y11 - y10;
      float dyz = y10 - y00;
      yTot = y10 + dyx*dx0 + dyz*(1.0-dz0)*dx0;
    }
  return yTot;
}


TerrainPatch::~TerrainPatch(){
  
  //cout << "Deleting patch x= " << xGrid << " y= " << yGrid << endl;
  
  if(hasGeometry()) {
  
    if(geometry->vertexArray != NULL)
      free(geometry->vertexArray);   

    if(geometry->texCoordArray != NULL)
      free(geometry->texCoordArray); 

    if(geometry->normalArray != NULL)
      free(geometry->normalArray);   

    if(geometry->indexArray != NULL)
      free(geometry->indexArray);
	  
	  glDeleteBuffers(1, &geometry->vb);
	  glDeleteBuffers(1, &geometry->ib);
	  glDeleteBuffers(1, &geometry->nb);
	  if (geometry->texCoordArray != NULL)
		  glDeleteBuffers(1, &geometry->tb);
		  
		
	  glDeleteVertexArrays(1, &geometry->vao);
    free(geometry);
	  
	       
  }
  
  objects.clear();
  rawHeightMap.clear();
  blendedHeightMap.clear();
  delete patchGenerator;
  
}


void TerrainPatch::draw(class Camera* cam,float time){//mat4 cameraMatrix,float time){

  if(hasGeometry() && cam->isInFrustum(this)){
    mat4 cameraMatrix = cam->cameraMatrix;
    mat4 modelView = T(xPos,0, yPos);
    // Draw terrain normally
    glUseProgram(*terrainShader);
    glUniformMatrix4fv(glGetUniformLocation(*terrainShader, "mdl2World"), 1, GL_TRUE, modelView.m);
    glUniformMatrix4fv(glGetUniformLocation(*terrainShader, "world2View"), 1, GL_TRUE, cameraMatrix.m);
    DrawModel(geometry, *terrainShader, "inPosition", "inNormal","inTexCoord"); 

		vec3 camPos = cam -> getPosition();
#if GRASS == 1
		// Check if the patch is close, else don't draw grass		
		camPos.y = 0.0;
		vec3 terrainPos = vec3(xPos+blendedSize/2,0.0,yPos+blendedSize/2);
		float maxDist = 500.0 + sqrt(2.0)*((float)(blendedSize/2));
		if(Norm(camPos-terrainPos) < maxDist){
		  // Draw grass

      glEnable(GL_POLYGON_SMOOTH); 
		  glUseProgram(*grassShader);
		  glUniformMatrix4fv(glGetUniformLocation(*grassShader, "mdl2World"), 1, GL_TRUE, modelView.m);
		  glUniformMatrix4fv(glGetUniformLocation(*grassShader, "world2View"), 1, GL_TRUE, cameraMatrix.m);
		  glUniform1f(glGetUniformLocation(*grassShader,"time"), time); 
		  DrawModel(geometry, *grassShader, "inPosition", "inNormal","inTexCoord");
      glDisable(GL_POLYGON_SMOOTH);		
}
#endif

    // Draw all objects 
    for(int i = 0;i < objects.size();i++){
      Plant* plant = (Plant*)objects.at(i);
      if(cam->isInFrustum(plant)){
        plant->draw(cameraMatrix,camPos,time);
      }
    }
  }
  else {
    //printf("Warning, patch (%i,%i) has no geometry to draw.\n",xGrid,yGrid); 
  }
}

bool TerrainPatch::hasGeometry(){
  
  return (geometryBoolean);
}
