#include "PerlinPatchGenerator.h"

//Constructor
PerlinPatchGenerator::PerlinPatchGenerator(int inputBiotope, int inputNoF, int inputAmplitude, int inputSize){
    biotope = inputBiotope;
    NoF = inputNoF;
    amplitudeScale = inputAmplitude;
    gridSize = inputSize;
}

//Prints any matrix in matrix form, not as a vector
void PerlinPatchGenerator::printMatrix(vector<float> matrix){
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
vector<float> PerlinPatchGenerator::addMatrices(vector<float> inGrid1, vector<float> inGrid2){

	for(int index = 0; index < gridSize * gridSize; index++){
		inGrid1[index] = inGrid1.at(index) + inGrid2.at(index);
	}

	return inGrid1;
}
//Interpolates a value from a and b, with x being the distance from a to the point
float PerlinPatchGenerator::interpolateValues(float a, float b, float x){

    //float Sx = 3.0*pow(x,2.0)-2.0*pow(x,3.0);
    //float res = a+Sx*(b-a);
    
    float res = a+x*(b-a); //Linear blending works best.
    return res;

}

//Creates a matrix(vector) with gradients. gradientPoints are the amount of elements along each axis
vector<vector<float>> PerlinPatchGenerator::createGradients(int gradientPoints) {

	vector<vector<float>> gradients;
    float value,value2,norm;
    
    
	for(int row = 0; row < gradientPoints; row++) {
		for(int col = 0; col < gradientPoints; col++) {
            value = rand() / (float)INT_MAX;
            value2 = rand() / (float)INT_MAX;
            norm = (float)sqrt(value * value + value2 * value2);
            value = value/norm;
            value2 = value2/norm;
            vector<float> tempVec;
            tempVec.push_back(value);
            tempVec.push_back(value2);
            gradients.push_back(tempVec);
       	} 
	}
    
	return gradients;
}

float PerlinPatchGenerator::dotProduct(vector<float> a,float b[2]){

    float dp = 0.0f;
    for (int i=0;i<a.size();i++)
        dp += a.at(i) * b[i];
    return dp;
}


//Creates a full patch for one frequency. Calculates gradientpoints, and interpolates values for all pixels between them
vector<float> PerlinPatchGenerator::createPatch(int frequency, int gradientPoints, float amplitude){
    
	vector<vector<float>> gradients;
	gradients = createGradients(gradientPoints);
    
    int numberOfPixels = gridSize/frequency;
    float s,t,u,v,diffX,diffY,a,b,z;
    int gradientX, gradientY;
    vector<float> finalGrid;
    float sVec[2], tVec[2], uVec[2], vVec[2];
            
	for(int row = 0; row < gridSize; row++) {
		diffY = (float)(row % numberOfPixels)/(float)numberOfPixels;

		for(int col = 0; col < gridSize; col++) {
			diffX = (float)(col % numberOfPixels)/(float)numberOfPixels;            

            //Check what gradientpoints we should interpolate values from
			gradientX = floor(col/numberOfPixels);
			gradientY = floor(row/numberOfPixels);

            //Calculate vectors from each gradientpoint to the pixel
            sVec[0] = diffX;
            sVec[1] = diffY;
            tVec[0] = diffX-1;
            tVec[1] = diffY;
            uVec[0] = diffX;
            uVec[1] = diffY-1;
            vVec[0] = diffX-1;
            vVec[1] = diffY-1;

            //Calculate values to interpolate between for each pixel
            s = dotProduct(gradients.at(gradientY*gradientPoints + gradientX),sVec);
            t = dotProduct(gradients.at(gradientY*gradientPoints + gradientX+1),tVec);
            u = dotProduct(gradients.at((gradientY+1)*gradientPoints + gradientX),uVec);
            v = dotProduct(gradients.at((gradientY+1)*gradientPoints + gradientX+1),vVec);

            //Interpolate in x-axis and y-axis
            a = interpolateValues(s,t,diffX); 
            b = interpolateValues(u,v,diffX);
            z = interpolateValues(a,b,diffY);;
            
			finalGrid.push_back(z * amplitude);
            
		} 
	}   
	return finalGrid;	
}

//Generates a full patch of value noise. Parameters can be set in constructor.
vector<float> PerlinPatchGenerator::generatePatch(int x, int y)
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

	for(int n = 0; n <= NoF; n = n+1){ //max value on n: 2^n <= size
		frequency = pow(2,n);
		gradientPoints = frequency + 1;
    	amplitude = 1.0/(float)frequency;

        if(biotope == 2){
            amplitude *= amplitude;
        }
        amplitude *= amplitudeScale;

		tempPatch = createPatch(frequency,gradientPoints, amplitude);
		heightMapPatch = addMatrices(heightMapPatch, tempPatch);
      
	}
	return heightMapPatch;
}

