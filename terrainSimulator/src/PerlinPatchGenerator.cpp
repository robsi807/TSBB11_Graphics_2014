#include "PerlinPatchGenerator.h"

PerlinPatchGenerator::PerlinPatchGenerator(){

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

	/*float ft = x*3.1415927;
	float f = (1.0-cos(ft))*0.5;
	float res = a*(1.0-f) + b*f;
    */
    float Sx = 3.0*pow(1.0*x,2.0)-2.0*pow(1.0*x,3.0);
    float res = a+Sx*(b-a);
    return res;

}

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

float PerlinPatchGenerator::dotProduct(vector<float> a,vector<float> b){

    float dp = 0.0f;
    for (int i=0;i<a.size();i++)
        dp += a.at(i) * b.at(i);
    return dp;
}

vector<float> PerlinPatchGenerator::createPatch(int gridSize, int frequency, int gradientPoints, float amplitude){
    cout << "createPatch begin\n";

	vector<vector<float>> gradients;
	gradients = createGradients(gradientPoints);

    int numberOfPixels = gridSize/frequency;
    float s,t,u,v,diffX,diffY,Sx,Sy,a,b,z,ft,f;
    int gradientX, gradientY;
    vector<float> finalGrid;

	for(int row = 0; row < gridSize; row++) {
		diffY = (float)(row % numberOfPixels)/(float)numberOfPixels;

		for(int col = 0; col < gridSize; col++) {
			diffX = (float)(col % numberOfPixels)/(float)numberOfPixels;            

			gradientX = floor(col/numberOfPixels);
			gradientY = floor(row/numberOfPixels);

            vector<float> sVec, tVec, uVec, vVec;
            sVec.push_back(diffX);
            sVec.push_back(diffY);
            tVec.push_back(diffX-1);
            tVec.push_back(diffY);
            uVec.push_back(diffX);
            uVec.push_back(diffY-1);
            vVec.push_back(diffX-1);
            vVec.push_back(diffY-1);

            s = dotProduct(gradients.at(gradientY*gradientPoints + gradientX),sVec);
            t = dotProduct(gradients.at(gradientY*gradientPoints + gradientX+1),tVec);
            u = dotProduct(gradients.at((gradientY+1)*gradientPoints + gradientX),uVec);
            v = dotProduct(gradients.at((gradientY+1)*gradientPoints + gradientX+1),vVec);

            a = interpolateValues(s,t,diffX); 
            b = interpolateValues(u,v,diffX);
            z = interpolateValues(a,b,diffY);;
            
            
            
			finalGrid.push_back(z * amplitude);
             

		} 
	}
    cout << "createPatch end\n";

	return finalGrid;	
}

vector<float> PerlinPatchGenerator::generatePatch(int x, int y, int size)
{
    cout << "generatePatch begin\n";
	vector<float> tempPatch;
	vector<float> heightMapPatch;
	int frequency;
	int gradientPoints;
	float amplitude;
    
	heightMapPatch.assign(size*size,0);

	for(int n = 0; n <= 7; n = n+1){ //max value on n: 2^n <= size
		frequency = pow(2,n);
		gradientPoints = frequency + 1;
    	amplitude = 1.0/(float)frequency;

		tempPatch = createPatch(size,frequency,gradientPoints, amplitude);
		heightMapPatch = addMatrices(heightMapPatch, tempPatch, size);
      
	}
      //printMatrix(heightMapPatch,size);
    cout << "generatePatch end\n";
	return heightMapPatch;
}

