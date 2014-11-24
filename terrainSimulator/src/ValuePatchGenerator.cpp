#include "ValuePatchGenerator.h"

ValuePatchGenerator::ValuePatchGenerator(int inputBiotope, int inputNoF, int inputAmplitude, int inputSize){
    biotope = inputBiotope;
    NoF = inputNoF;
    amplitudeScale = inputAmplitude;
    gridSize = inputSize;
}

void ValuePatchGenerator::printMatrix(vector<float> matrix){
	printf("[");
	for(int row = 0; row < gridSize; row++) {
		for(int col = 0; col < gridSize; col++) {
			printf("%1.3f  ", matrix.at(row*gridSize + col));
		} 
		printf(";");
	} 
	printf("]");
}

vector<float> ValuePatchGenerator::addMatrices(vector<float> inGrid1, vector<float> inGrid2){

	vector<float> outGrid;
	for(int index = 0; index < gridSize * gridSize; index++){
		outGrid.push_back(inGrid1.at(index) + inGrid2.at(index));
	}
	return outGrid;
}

float ValuePatchGenerator::interpolateValues(float a, float b, float x){

	/*float ft = x*3.1415927;
	float f = (1.0-cos(ft))*0.5;
	float res = a*(1.0-f) + b*f;
    */
    
    float Sx = 3.0*pow(x,2.0)-2.0*pow(x,3.0);
    float res = a+Sx*(b-a);

    //float res = a+x*(b-a);
    return res;

}

vector<float> ValuePatchGenerator::createGradients(int gradientPoints) {

	vector<float> gradients;
    float value;
    
	for(int row = 0; row < gradientPoints; row++) {
		for(int col = 0; col < gradientPoints; col++) {
            value = (rand() / (float)INT_MAX)*(rand() / (float)INT_MAX);
            gradients.push_back(value);
       	} 
	}
	return gradients;
}

vector<float> ValuePatchGenerator::createPatch(int frequency, int gradientPoints, float amplitude){

	vector<float> gradients;
	gradients = createGradients(gradientPoints);
    int numberOfPixels = gridSize/frequency;

    vector<float> finalGrid;

	for(int row = 0; row < gridSize; row++) {
		float diffY = (float)(row % numberOfPixels)/(float)numberOfPixels;

		for(int col = 0; col < gridSize; col++) {
			float diffX = (float)(col % numberOfPixels)/(float)numberOfPixels;

			int gradientX = floor(col/numberOfPixels);
			int gradientY = floor(row/numberOfPixels);

			float interpolatedX1 = interpolateValues(gradients.at(gradientY*gradientPoints 	+ gradientX),
 													 gradients.at((gradientY)*gradientPoints + gradientX+1),
													 diffX);
			float interpolatedX2 = interpolateValues(gradients.at((gradientY+1)*gradientPoints + gradientX),
													 gradients.at((gradientY+1)*gradientPoints + gradientX+1),
													 diffX);
			float finalInterp = interpolateValues(interpolatedX1,interpolatedX2,diffY);
			finalGrid.push_back(finalInterp * amplitude);

		} 
	}

	return finalGrid;	
}

vector<float> ValuePatchGenerator::generatePatch(int x, int y)
{
	vector<float> tempPatch;
	vector<float> heightMapPatch;
	int frequency;
	int gradientPoints;
	float amplitude;
    
	heightMapPatch.assign(gridSize*gridSize,0);
    
    //Position based seed
    int n=x+y*57;
    n=(n<<13)^n;
    int seed=(n*(n*n*60493+19990303)+1376312589)&0x7fffffff;
    srand(seed);

	for(int n = 1; n <= NoF; n = n+2){ //max value on n: 2^n <= size

		frequency = pow(2,n);
		gradientPoints = frequency + 1;
    	amplitude = 1.0/((float)frequency);

        if(biotope == 2){
            amplitude *= amplitude;
        }
        amplitude *= amplitudeScale;

		tempPatch = createPatch(frequency,gradientPoints, amplitude);
		heightMapPatch = addMatrices(heightMapPatch, tempPatch);
 
	}
	return heightMapPatch;
}

