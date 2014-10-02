#include "TerrainPatch.h"

TerrainPatch::TerrainPatch(TextureData *tex, int x, int y, GLuint *phongShader, char *imagePath, Model *terrain) : heightMap(tex), posX(x), posY(y){ 

		geometry = terrain;
		shader = phongShader;
		//glActiveTexture(GL_TEXTURE0);

		//LoadTGATextureSimple(imagePath, &texture);

		//glUniform1i(glGetUniformLocation(*shader, "tex"), 0); // Texture unit 0
		generateGeometry();
}

vec3 TerrainPatch::calcNormal(vec3 v0, vec3 v1, vec3 v2)
{
		vec3 n = CrossProduct(VectorSub(v1,v0),VectorSub(v2,v0));
		if(n.y < 0)
				n = ScalarMult(n,-1);
		return Normalize(n);
}


void TerrainPatch::generateGeometry(){
		int vertexCount = heightMap->width * heightMap->height;
		int triangleCount = (heightMap->width-1) * (heightMap->height-1) * 2;
		int x, z;

		GLfloat vertexArray[3 * vertexCount];
		GLfloat vertexArray2[3*vertexCount];
		GLfloat normalArray[3 * vertexCount];
		GLfloat texCoordArray[ 2 * vertexCount];
		GLuint indexArray[3 * triangleCount];

		float hScale = 20.0;

		for (x = 0; x < heightMap->width; x++)
				for (z = 0; z < heightMap->height; z++)
				{
						vec3 n = vec3(0.0,1.0,0.0);
						if(!((x==0)||(x==heightMap->width - 1)||(z == 0)||(z==heightMap->height - 1)))
						{
								float x0 = (float)(x-1);
								float z0 = (float)(z-1);

								float x1 = (float)x;
								float z1 = (float)z;

								float x2 = (float)(x+1);
								float z2 = (float)(z+1);

								float y00 = heightMap->imageData[((int)x0 + (int)z0 * heightMap->width) * (heightMap->bpp/8)] / hScale;
								float y01 = heightMap->imageData[((int)x1 + (int)z0 * heightMap->width) * (heightMap->bpp/8)] / hScale;

								float y10 = heightMap->imageData[((int)x0 + (int)z1 * heightMap->width) * (heightMap->bpp/8)] / hScale;
								float y11 = heightMap->imageData[((int)x1 + (int)z1 * heightMap->width) * (heightMap->bpp/8)] / hScale;
								float y12 = heightMap->imageData[((int)x2 + (int)z1 * heightMap->width) * (heightMap->bpp/8)] / hScale;

								float y21 = heightMap->imageData[((int)x1 + (int)z2 * heightMap->width) * (heightMap->bpp/8)] / hScale;
								float y22 = heightMap->imageData[((int)x2 + (int)z2 * heightMap->width) * (heightMap->bpp/8)] / hScale;

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
						}

						vertexArray2[(x + z * heightMap->width)*3 + 0] = x / 1.0;
						vertexArray2[(x + z * heightMap->width)*3 + 1] = heightMap->imageData[(x + z * heightMap->width) * (heightMap->bpp/8)] / hScale;
						vertexArray2[(x + z * heightMap->width)*3 + 2] = z / 1.0;

						// Normal vectors. You need to calculate these.
						normalArray[(x + z * heightMap->width)*3 + 0] = n.x; //(y1-y0)*(z2-z0)-(y2-y0)*(z1-z0); //0.0;
						normalArray[(x + z * heightMap->width)*3 + 1] = n.y; //(z1-z0)*(x2-x0)-(z2-z0)*(x1-x0); //1.0;
						normalArray[(x + z * heightMap->width)*3 + 2] =n.z; //(x1-x0)*(y2-y0)-(x2-x0)*(y1-y0); //0.0;

						// Texture coordinates. You may want to scale them.
						texCoordArray[(x + z * heightMap->width)*2 + 0] = x; // (float)x / heightMap->width;
						texCoordArray[(x + z * heightMap->width)*2 + 1] = z; // (float)z / heightMap->height;
				}
		for (x = 0; x < heightMap->width-1; x++)
				for (z = 0; z < heightMap->height-1; z++)
				{
						// Triangle 1
						indexArray[(x + z * (heightMap->width-1))*6 + 0] = x + z * heightMap->width;
						indexArray[(x + z * (heightMap->width-1))*6 + 1] = x + (z+1) * heightMap->width;
						indexArray[(x + z * (heightMap->width-1))*6 + 2] = x+1 + z * heightMap->width;
						// Triangle 2
						indexArray[(x + z * (heightMap->width-1))*6 + 3] = x+1 + z * heightMap->width;
						indexArray[(x + z * (heightMap->width-1))*6 + 4] = x + (z+1) * heightMap->width;
						indexArray[(x + z * (heightMap->width-1))*6 + 5] = x+1 + (z+1) * heightMap->width;
				}



		geometry->vertexArray = vertexArray2;
		geometry->normalArray = normalArray;
		geometry->texCoordArray = texCoordArray;
		geometry->indexArray = indexArray;
		geometry->numVertices = vertexCount;
		geometry->numIndices = triangleCount*3;

		/*geometry = LoadDataToModel(
		  vertexArray,
		  normalArray,
		  texCoordArray,
		  NULL,
		  indexArray,
		  vertexCount,
		  triangleCount*3);*/

}



float TerrainPatch::calcHeight(float x,float z,int texWidth)
{
		int x0,x1,z0,z1;
		float y00,y01,y10,y11,dx0,dz0,yTot;
		x0 = floor(x); 
		x1 = ceil(x); 
		z0 = floor(z); 
		z1 = ceil(z);
		dx0 = x - x0;
		dz0 = z - z0;

		y00 = geometry->vertexArray[(x0 + z0 * texWidth)*3 + 1];
		y01 = geometry->vertexArray[(x1 + z0 * texWidth)*3 + 1];
		y10 = geometry->vertexArray[(x0 + z1 * texWidth)*3 + 1];
		y11 = geometry->vertexArray[(x1 + z1 * texWidth)*3 + 1];

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

		mat4 modelView = T(posX,0, posY);
		glUseProgram(*shader);
		glUniformMatrix4fv(glGetUniformLocation(*shader, "mdl2World"), 1, GL_TRUE, modelView.m);
		glUniformMatrix4fv(glGetUniformLocation(*shader, "world2View"), 1, GL_TRUE, cameraMatrix.m);
		//glBindTexture(GL_TEXTURE_2D, texture);	
		DrawModel(geometry, *shader, "inPosition", "inNormal", "inTexCoord"); 

}
