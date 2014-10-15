#include "PerlinPatchGenerator.h"

PerlinPatchGenerator::PerlinPatchGenerator(){

int INT_MAX = std::numeric_limits<unsigned int>::max();

  
}

void PerlinPatchGenerator::printMatrix(vector<float> matrix, int matrixSize){
	printf("[");
	for(int row = 0; row < matrixSize; row++) {
		for(int col = 0; col < matrixSize; col++) {
			printf("%1.3f  ", matrix.at(row*matrixSize + col));
		} 
		printf(";");
	} 
	printf("]");
}

vector<float> PerlinPatchGenerator::addMatrices(vector<float> inGrid1, vector<float> inGrid2, int gridSize){

	vector<float> outGrid;
	for(int index = 0; index < gridSize * gridSize; index++){
		outGrid.push_back(inGrid1.at(index) + inGrid2.at(index));
	}
	return outGrid;
}

float PerlinPatchGenerator::interpolateValues(float a, float b, float x){

	float ft = x*3.1415927;
	float f = (1.0-cos(ft))*0.5;
	float res = a*(1.0-f) + b*f;
	return res;

}

vector<float> PerlinPatchGenerator::createGradients(int gradientPoints) {

	vector<float> gradients;

	for(int row = 0; row < gradientPoints; row++) {
		for(int col = 0; col < gradientPoints; col++) {
			gradients.push_back(rand() / (float)INT_MAX);
		} 
	}
	return gradients;
}

vector<float> PerlinPatchGenerator::createPatch(int gridSize, float frequency, int gradientPoints, float amplitude){
    //printf("begin createPatch\n");

	vector<float> gradients;
	//printf("begin createGradients\n");
    gradients = createGradients(gradientPoints);
    //printf("end createGradients\n");	
    int numberOfPixels = gridSize/frequency;
    //printf("numberOfPixels %d", numberOfPixels);
	
    vector<float> finalGrid;

	for(int row = 0; row < gridSize; row++) {
		float diffX = (float)(row % numberOfPixels)/numberOfPixels;

		for(int col = 0; col < gridSize; col++) {
			float diffY = (float)(col % numberOfPixels)/numberOfPixels;

			int gradientX = floor(row/numberOfPixels);
			int gradientY = floor(col/numberOfPixels);

			float interpolatedX1 = interpolateValues(gradients.at(gradientX*gradientPoints 	+ gradientY),
 													 gradients.at((gradientX+1)*gradientPoints + gradientY),
													 diffX);
			float interpolatedX2 = interpolateValues(gradients.at(gradientX*gradientPoints 	+ gradientY+1),
													 gradients.at((gradientX+1)*gradientPoints + gradientY+1),
													 diffX);
			
			finalGrid.push_back(interpolateValues(interpolatedX1,interpolatedX2,diffY) * amplitude);
			
		} 

	}
    //printf("end createPatch\n");
 
	return finalGrid;	
}

vector<float> PerlinPatchGenerator::initHeightMap(vector<float> heightMap, int gridSize) {

	for(int index = 0;index < gridSize*gridSize;index++) {
		heightMap.push_back(0.0f);
	}
	return heightMap;
}

vector<float> PerlinPatchGenerator::generatePatch(int x, int y, int size)
{
	vector<float> tempPatch;
	vector<float> heightMapPatch;
	float frequency;
	int gradientPoints;
	float amplitude;
    
	heightMapPatch = initHeightMap(heightMapPatch, size);

	for(int n = 2; n <= 6; n++){ //max value on n: 2^n <= size
		frequency = pow(2,n);
		gradientPoints = frequency + 1;
    	amplitude = 1.0/frequency;

		tempPatch = createPatch(size,frequency,gradientPoints, amplitude);  		
		heightMapPatch = addMatrices(heightMapPatch, tempPatch, size);

        
	}
	return heightMapPatch;
}

