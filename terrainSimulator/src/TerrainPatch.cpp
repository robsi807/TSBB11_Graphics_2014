#include "TerrainPatch.h"

TerrainPatch::TerrainPatch(vector<float> tex, int patchSize, int x, int y, int overlap, GLuint* phongShader, GLuint *terrainTexture) : rawHeightMap(tex), blendedHeightMap(tex), xGrid(x), yGrid(y), size(patchSize), patchOverlap(overlap){ 
  
  blendedSize = patchSize-overlap+1;

  // Calculate positions
  xPos = x * (patchSize - overlap);
  yPos = y * (patchSize - overlap);

  shader = phongShader;
  texture = terrainTexture;
  
  //generateGeometry();
  geometry = NULL;
  geometryBoolean = false;

}

vec3 TerrainPatch::calcNormal(vec3 v0, vec3 v1, vec3 v2)
{
  vec3 n = CrossProduct(VectorSub(v1,v0),VectorSub(v2,v0));
  if(n.y < 0)
    n = ScalarMult(n,-1);
  return Normalize(n);
}


void TerrainPatch::generateGeometry(){
  if(!hasGeometry()){
    int blendedWidth = size-patchOverlap+1;
    int blendedHeight = size-patchOverlap+1;
    int offset = patchOverlap/2;

    int vertexCount = blendedWidth * blendedHeight;
    int triangleCount = (blendedWidth-1) * (blendedHeight-1) * 2;

    int x, z;

    GLfloat *vertexArray = (GLfloat *)malloc(sizeof(GLfloat) * 3 * vertexCount);
    GLfloat *normalArray = (GLfloat *)malloc(sizeof(GLfloat) * 3 * vertexCount);
    GLfloat *texCoordArray = (GLfloat *)malloc(sizeof(GLfloat) * 2 * vertexCount);
    GLuint *indexArray = (GLuint *)malloc(sizeof(GLuint) * triangleCount*3);
    // GLfloat vertexArray[3 * vertexCount];
    // GLfloat normalArray[3 * vertexCount];
    // GLfloat texCoordArray[ 2 * vertexCount];
    // GLuint indexArray[3 * triangleCount];

    float heightScale = 0.0025;

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

	    float y00 = blendedHeightMap.at((x0 + z0 * size)) / heightScale;
	    float y01 = blendedHeightMap.at((x1 + z0 * size)) / heightScale;

	    float y10 = blendedHeightMap.at((x0 + z1 * size)) / heightScale;
	    float y11 = blendedHeightMap.at((x1 + z1 * size)) / heightScale;
	    float y12 = blendedHeightMap.at((x2 + z1 * size)) / heightScale;

	    float y21 = blendedHeightMap.at((x1 + z2 * size)) / heightScale;
	    float y22 = blendedHeightMap.at((x2 + z2 * size)) / heightScale;

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

	    vertexArray[(x + z * blendedWidth)*3 + 0] = x1 / 1.0;
	    vertexArray[(x + z * blendedWidth)*3 + 1] = blendedHeightMap.at((x1 + z1 * size)) / heightScale;
	    vertexArray[(x + z * blendedWidth)*3 + 2] = z1 / 1.0;

	    // Normal vectors. You need to calculate these.
	    normalArray[(x + z * blendedWidth)*3 + 0] = n.x; //(y1-y0)*(z2-z0)-(y2-y0)*(z1-z0); //0.0;
	    normalArray[(x + z * blendedWidth)*3 + 1] = n.y; //(z1-z0)*(x2-x0)-(z2-z0)*(x1-x0); //1.0;
	    normalArray[(x + z * blendedWidth)*3 + 2] = n.z; //(x1-x0)*(y2-y0)-(x2-x0)*(y1-y0); //0.0;

	    // Texture coordinates. You may want to scale them.
	    texCoordArray[(x + z * blendedWidth)*2 + 0] = ((float)x1); // (float)x / size;
	    texCoordArray[(x + z * blendedWidth)*2 + 1] = ((float)z1); // (float)z / patchHeight;
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
	
  }
  
  
}

void TerrainPatch::uploadGeometry() {
  cout << "TerrainPatch::uploadGeometry: uploading geometry!\n";
  
	BuildModelVAO2(geometry);
	
  cout << "TerrainPatch::uploadGeometry: geometry DONE uploading! <3\n";
  geometryBoolean = true;
}

void TerrainPatch::generateAndUploadGeometry() {
  generateGeometry();
  uploadGeometry();
}


float TerrainPatch::calcHeight(float x,float z)
{
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
  std::cout << "TerrainPatch destructor is used, geometry is deleted\n";
  delete geometry;
}


void TerrainPatch::draw(mat4 cameraMatrix){

  if(hasGeometry()){
    mat4 modelView = T(xPos,0, yPos);
    glUseProgram(*shader);
    glUniformMatrix4fv(glGetUniformLocation(*shader, "mdl2World"), 1, GL_TRUE, modelView.m);
    glUniformMatrix4fv(glGetUniformLocation(*shader, "world2View"), 1, GL_TRUE, cameraMatrix.m);
    // glBindTexture(GL_TEXTURE_2D, *texture); 
    //glBindVertexArray(geometry->vao);
    
    
    
    DrawModel(geometry, *shader, "inPosition", "inNormal","inTexCoord"); 
  }
  else {
    //printf("Warning, patch (%i,%i) has no geometry to draw.\n",xGrid,yGrid); 
  }
}

bool TerrainPatch::hasGeometry(){
  
  return (geometryBoolean);
}
