#include "ValuePatchGenerator.h"

//Constructor
ValuePatchGenerator::ValuePatchGenerator(int inputBiotope, int inputNoF, int inputAmplitude, int inputSize){
    biotope = inputBiotope;
    NoF = inputNoF;
    amplitudeScale = inputAmplitude;
    gridSize = inputSize;
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
    
	for(int row = 0; row < gradientPoints; row++) {
		for(int col = 0; col < gradientPoints; col++) {
            value = (rand() / (float)INT_MAX)*(rand() / (float)INT_MAX);
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
		//Calculate distance from pixel to closest gradient point in Y-axis
		float diffY = (float)(row % numberOfPixels)/(float)numberOfPixels;

		for(int col = 0; col < gridSize; col++) {
    		//Calculate distance from pixel to closest gradient point in X-axis
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
    
    //Position based seed
    int n=x+y*57;
    n=(n<<13)^n;
    int seed=(n*(n*n*60493+19990303)+1376312589)&0x7fffffff;
    srand(seed);

	//Creates a height map for each frequency, and adds them together 
	for(int n = 1; n <= NoF; n = n+2){ 
        //Each frequency should be a multiple of the previous one  
		frequency = pow(2,n);
		gradientPoints = frequency + 1;
        //Amplitude decreases as frequency increases. Higher frequencies should have smaller inpact on the heightmap  
    	amplitude = 1.0/(float)frequency;

        // Biotope = 2 gives the patch desert like shape
        if(biotope == 2){
            amplitude *= amplitude;
        }
        amplitude *= amplitudeScale;

        //Calculate patch for that frequency
		tempPatch = createPatch(frequency,gradientPoints, amplitude);
        //Add the temporary patch together with previous patches
		heightMapPatch = addMatrices(heightMapPatch, tempPatch);
 
	}
	return heightMapPatch;
}

