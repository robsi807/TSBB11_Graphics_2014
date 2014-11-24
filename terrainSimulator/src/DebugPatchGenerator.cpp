#include "DebugPatchGenerator.h"

DebugPatchGenerator::DebugPatchGenerator(bool inDebugMode){
  debugMode = inDebugMode;

}

void DebugPatchGenerator::printMatrix(vector<float> matrix, int matrixSize){
	printf("[");
	for(int row = 0; row < matrixSize; row++) {
		for(int col = 0; col < matrixSize; col++) {
			printf("%1.3f  ", matrix.at(row*matrixSize + col));
		} 
		printf(";");
	} 
	printf("]");
}

vector<float> DebugPatchGenerator::addMatrices(vector<float> inGrid1, vector<float> inGrid2, int gridSize){

	vector<float> outGrid;
	for(int index = 0; index < gridSize * gridSize; index++){
		outGrid.push_back(inGrid1.at(index) + inGrid2.at(index));
	}
	return outGrid;
}

float DebugPatchGenerator::interpolateValues(float a, float b, float x){

	/*float ft = x*3.1415927;
	float f = (1.0-cos(ft))*0.5;
	float res = a*(1.0-f) + b*f;
    */
    float Sx = 3.0*pow(x,2.0)-2.0*pow(x,3.0);
    float res = a+Sx*(b-a);
    return res;

}

vector<float> DebugPatchGenerator::createGradients(int gradientPoints) {

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

vector<float> DebugPatchGenerator::createPatch(int gridSize, int frequency, int gradientPoints, float amplitude){

	vector<float> gradients;
	gradients = createGradients(gradientPoints);
	
	
  vector<float> finalGrid;


	
	
	
    int numberOfPixels = gridSize/frequency;

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
		  //myfile << "\n";
	  }

	return finalGrid;	
}

vector<float> DebugPatchGenerator::generatePatch(int x, int y, int size)
{
	vector<float> tempPatch;
	vector<float> heightMapPatch;
	int frequency;
	int gradientPoints;
	float amplitude;
	
	clock_t tstart, tend;
	
		
	cout << "DebugPatchGen: Trying to open a file!\n";
	
	fstream myfile;
	stringstream sstream;
	string fileName;
	sstream << "heightmaps/" << size << "-patch-" << x << "-" << y << ".txt";
	
	fileName = sstream.str();
	
	cout << "DebugPatchGen: Trying to open a file 2!\n";
	cout << fileName << "\n";
	//myfile.open(fileName, fstream::in | fstream::out | fstream::app);
	//myfile.open(fileName, fstream::in | fstream::out);
	myfile.open(fileName, fstream::in);
	
	
	/*
	if(myfile.is_open()) {
	  cout << "File is open!\n";
	  myfile << "hello world!";
	} else {
	  
	  myfile.open(fileName, fstream::out);
	  cout << "File is not open!\n";
	  myfile << "bye world!";
	}
	
	
	
	myfile.close();
	*/
	
	
	tstart = clock();
	
	if(!myfile.is_open()) {

	  //cout << "File is not open!\n";
	  if(debugMode) {
      myfile.open(fileName, fstream::out);
    }
	
	  heightMapPatch.assign(size*size,0);
      
    //Position based seed
    int n=x+y*57;
    n=(n<<13)^n;
    int seed=(n*(n*n*60493+19990303)+1376312589)&0x7fffffff;
    srand(seed);

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
	  
	  
	  if(debugMode) {
	    for(int i = 0; i < heightMapPatch.size();i++) {
	      
			    myfile << heightMapPatch.at(i) << "\n";
	    } 
	  }
	   // don't write to file, to check how long time it takes to generate a patch only!
	} else {
	  cout << "We have a file! Skipping loading models ;)\n";
	  
	  
	  cout << "File already exists, reading patches!\n";
	  string line;
	  while(getline(myfile, line)) {
	    heightMapPatch.push_back(stof(line));
	  }
	  
	  
	}
	
	
	myfile.close();
	
	tend = clock();
	
	cout << "generating a patch takes: " << 1000*((double) (tend - tstart))/CLOCKS_PER_SEC << " milliseconds. \n";
	
	return heightMapPatch;
}

