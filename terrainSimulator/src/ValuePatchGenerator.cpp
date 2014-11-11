#include "ValuePatchGenerator.h"

ValuePatchGenerator::ValuePatchGenerator(){

}

void ValuePatchGenerator::printMatrix(vector<float> matrix, int matrixSize){
	printf("[");
	for(int row = 0; row < matrixSize; row++) {
		for(int col = 0; col < matrixSize; col++) {
			printf("%1.3f  ", matrix.at(row*matrixSize + col));
		} 
		printf(";");
	} 
	printf("]");
}

vector<float> ValuePatchGenerator::addMatrices(vector<float> inGrid1, vector<float> inGrid2, int gridSize){

	vector<float> outGrid;
	for(int index = 0; index < gridSize * gridSize; index++){
		outGrid.push_back(inGrid1.at(index) + inGrid2.at(index));
	}
	return outGrid;
}

float ValuePatchGenerator::interpolateValues(float a, float b, float x){

	float ft = x*3.1415927;
	float f = (1.0-cos(ft))*0.5;
	float res = a*(1.0-f) + b*f;
    
    /*float Sx = 3.0*pow(1.0*x,2.0)-2.0*pow(1.0*x,3.0);
    float res = a+Sx*(b-a);*/
    return res;

}

vector<float> ValuePatchGenerator::createGradients(int gradientPoints) {

	vector<float> gradients;
    float value = 0;
    
	for(int row = 0; row < gradientPoints; row++) {
		for(int col = 0; col < gradientPoints; col++) {
            value = value + (rand() / (float)INT_MAX)*(rand() / (float)INT_MAX);
            gradients.push_back(value);
       	} 
	}
	return gradients;
}

vector<float> ValuePatchGenerator::createPatch(int gridSize, int frequency, int gradientPoints, float amplitude){

	vector<float> gradients;
	gradients = createGradients(gradientPoints);
    int numberOfPixels = gridSize/frequency;

    vector<float> finalGrid;

	for(int row = 0; row < gridSize; row++) {
		float diffX = (float)(row % numberOfPixels)/(float)numberOfPixels;

		for(int col = 0; col < gridSize; col++) {
			float diffY = (float)(col % numberOfPixels)/(float)numberOfPixels;

			int gradientX = floor(row/numberOfPixels);
			int gradientY = floor(col/numberOfPixels);

			float interpolatedX1 = interpolateValues(gradients.at(gradientX*gradientPoints 	+ gradientY),
 													 gradients.at((gradientX+1)*gradientPoints + gradientY),
													 diffX);
			float interpolatedX2 = interpolateValues(gradients.at(gradientX*gradientPoints 	+ gradientY+1),
													 gradients.at((gradientX+1)*gradientPoints + gradientY+1),
													 diffX);
			float finalInterp = interpolateValues(interpolatedX1,interpolatedX2,diffY);
			finalGrid.push_back(finalInterp * amplitude);

		} 
	}

	return finalGrid;	
}

vector<float> ValuePatchGenerator::generatePatch(int x, int y, int size)
{
	vector<float> tempPatch;
	vector<float> heightMapPatch;
	int frequency;
	int gradientPoints;
	float amplitude;
    
	heightMapPatch.assign(size*size,0);
    srand(pow(x+y,y)/x);

	for(int n = 2; n <= 8; n = n+2){ //max value on n: 2^n <= size

        /*Biotopes:

        Landscape: In the for loop, use n = n+2 instead of n++;        
        Sand: Set amplitude = amplitude^2:
        */

		frequency = pow(2,n);
		gradientPoints = frequency + 1;
    	amplitude = 1.0/((float)frequency);

		tempPatch = createPatch(size,frequency,gradientPoints, amplitude);
		heightMapPatch = addMatrices(heightMapPatch, tempPatch, size);
 
	}
	return heightMapPatch;
}

