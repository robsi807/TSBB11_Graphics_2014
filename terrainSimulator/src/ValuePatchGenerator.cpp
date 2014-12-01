#include "ValuePatchGenerator.h"

//Constructor
ValuePatchGenerator::ValuePatchGenerator(int inputBiotope, int inputNoF, int inputAmplitude, int inputSize,int x,int y){
    biotope = inputBiotope;
    NoF = inputNoF;
    amplitudeScale = inputAmplitude;
    gridSize = inputSize;

    //Position based seed
    int n=x+y*57;
    n=(n<<13)^n;
    seed=(n*(n*n*60493+19990303)+1376312589)&0x7fffffff;
    rng.seed(seed);
}

//Prints any matrix in matrix form, not as a vector
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

//Add two matrices(vectors) together
vector<float> ValuePatchGenerator::addMatrices(vector<float> inGrid1, vector<float> inGrid2){

	vector<float> outGrid;
	for(int index = 0; index < gridSize * gridSize; index++){
		outGrid.push_back(inGrid1.at(index) + inGrid2.at(index));
	}
	return outGrid;
}

//Interpolates a value from a and b, with x being the distance from a to the point
float ValuePatchGenerator::interpolateValues(float a, float b, float x){

    float Sx = 3.0*pow(x,2.0)-2.0*pow(x,3.0);
    float res = a+Sx*(b-a);
    return res;
}

//Creates a matrix(vector) with values to interpolate between. gradientPoints are the amount of elements along each axis
vector<float> ValuePatchGenerator::createGradients(int gradientPoints) {
	vector<float> gradients;
    float value;
 
    boost::uniform_int<> one_to_six( 0, INT_MAX );
    boost::variate_generator<RNGType, boost::uniform_int<>> dice(rng, one_to_six);
	
  for(int row = 0; row < gradientPoints; row++) {
		for(int col = 0; col < gradientPoints; col++) {
            value = (dice() / (float)INT_MAX)*(dice()/(float)INT_MAX);
            gradients.push_back(value);
       	} 
	}
	return gradients;
}

//Creates a full patch for one frequency. Calculates gradientpoints, and interpolates values for all pixels between them
vector<float> ValuePatchGenerator::createPatch(int frequency, int gradientPoints, float amplitude){

	vector<float> gradients;
	gradients = createGradients(gradientPoints);
    int numberOfPixels = gridSize/frequency;

    vector<float> finalGrid;

	for(int row = 0; row < gridSize; row++) {
		float diffY = (float)(row % numberOfPixels)/(float)numberOfPixels;

		for(int col = 0; col < gridSize; col++) {
			float diffX = (float)(col % numberOfPixels)/(float)numberOfPixels;

            //Check what gradientpoints we should interpolate values from
			int gradientX = floor(col/numberOfPixels); 
			int gradientY = floor(row/numberOfPixels);

            //Interpolates i x-axis            
			float interpolatedX1 = interpolateValues(gradients.at(gradientY*gradientPoints 	+ gradientX),
 													 gradients.at((gradientY)*gradientPoints + gradientX+1),
													 diffX);
			float interpolatedX2 = interpolateValues(gradients.at((gradientY+1)*gradientPoints + gradientX),
													 gradients.at((gradientY+1)*gradientPoints + gradientX+1),
													 diffX);
            //Interpolates in y(z)-axis			
            float finalInterp = interpolateValues(interpolatedX1,interpolatedX2,diffY);
			finalGrid.push_back(finalInterp * amplitude);

		} 
	}
  gradients.clear();
	return finalGrid;	
}

//Generates a full patch of value noise. Parameters can be set in constructor.
vector<float> ValuePatchGenerator::generatePatch(int x, int y)
{
	vector<float> tempPatch;
	vector<float> heightMapPatch;
	int frequency;
	int gradientPoints;
	float amplitude;
    
	heightMapPatch.assign(gridSize*gridSize,0);
    
  //srand(seed);

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
	tempPatch.clear();
	return heightMapPatch;
}

