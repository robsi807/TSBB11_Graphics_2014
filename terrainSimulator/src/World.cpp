#include "World.h"

World::World(){
  init();
}

// ------------ FULKOD -----------------

void BuildModelVAO2(Model *m)
{
	glGenVertexArrays(1, &m->vao);
	glGenBuffers(1, &m->vb);
	glGenBuffers(1, &m->ib);
	glGenBuffers(1, &m->nb);
	if (m->texCoordArray != NULL)
		glGenBuffers(1, &m->tb);
	
	glBindVertexArray(m->vao);
	
	// VBO for vertex data
	glBindBuffer(GL_ARRAY_BUFFER, m->vb);
	glBufferData(GL_ARRAY_BUFFER, m->numVertices*3*sizeof(GLfloat), m->vertexArray, GL_STATIC_DRAW);
	//glVertexAttribPointer(glGetAttribLocation(program, vertexVariableName), 3, GL_FLOAT, GL_FALSE, 0, 0); 
	//glEnableVertexAttribArray(glGetAttribLocation(program, vertexVariableName));
	
	// VBO for normal data
	glBindBuffer(GL_ARRAY_BUFFER, m->nb);
	glBufferData(GL_ARRAY_BUFFER, m->numVertices*3*sizeof(GLfloat), m->normalArray, GL_STATIC_DRAW);
	//glVertexAttribPointer(glGetAttribLocation(program, normalVariableName), 3, GL_FLOAT, GL_FALSE, 0, 0);
	//glEnableVertexAttribArray(glGetAttribLocation(program, normalVariableName));
	
	// VBO for texture coordinate data NEW for 5b
	if (m->texCoordArray != NULL)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m->tb);
		glBufferData(GL_ARRAY_BUFFER, m->numVertices*2*sizeof(GLfloat), m->texCoordArray, GL_STATIC_DRAW);
		//glVertexAttribPointer(glGetAttribLocation(program, texCoordVariableName), 2, GL_FLOAT, GL_FALSE, 0, 0);
		//glEnableVertexAttribArray(glGetAttribLocation(program, texCoordVariableName));
	}
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->ib);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m->numIndices*sizeof(GLuint), m->indexArray, GL_STATIC_DRAW);
}




// ---------------- END OF FULKOD -----------------------



void addToVector(World *w, float patchX, float patchY, float patchSize){
  cout << "@top of addToVector\n";
	TextureData heightMap = w->patchGenerator->generatePatch(patchSize);

	TerrainPatch* terrainPatch = new TerrainPatch(&heightMap, patchX*patchSize , patchY*patchSize, &(w->phongShader),"../textures/grass.tga"); // TODO: dont load the texture for each patch
	w->generatedTerrain.push_back(terrainPatch);
  cout << "@addToVector, geometry = "<< terrainPatch->geometry << "\n";
}

void World::generatePatch(int patchX, int patchY, float patchSize){

  cout << "@ top of generatePatch\n";
  thread first (addToVector, this, patchX, patchY, patchSize);
  first.join();
}


void World::addGeneratedTerrain(){

  if(generatedTerrain.size() > 0){
    cout << "--- STARTED ADDING STUFF TO WORLD VECTOR ---\n";
    for(int i = 0; i < generatedTerrain.size(); i++){
      cout << "Adding patch @" << generatedTerrain.at(i)->posX << ", " << generatedTerrain.at(i)->posY << "\n";
      cout << "This patch has geometry @" << generatedTerrain.at(i)->geometry << "\n";
      terrainVector.push_back(generatedTerrain.at(i));
      BuildModelVAO2(generatedTerrain.at(i)->geometry);
    }
    cout << "--- END OF ADD TO VECTOR ---\n";
    cout << "terrainVector.size() = " << terrainVector.size() << ", generatedTerrain.size() = " << generatedTerrain.size() << " before clear\n";
    generatedTerrain.clear();
  }
}

void World::init(){

  // Load shaders
  phongShader = loadShaders("phong.vert", "phong.frag");
  skyboxShader = loadShaders("skybox.vert", "skybox.frag");

  // Init objects

  patchGenerator = new MockupPatchGenerator("../textures/fft-terrain.tga");

  camera = new Camera(vec3(24,20,24), 1, 7);
  skybox = new Skybox(&skyboxShader, camera->projectionMatrix, "../textures/skybox/sky%d.tga");

  // Init light
  glUseProgram(phongShader);

  vec3 lightSource = vec3(50.0f, 100.0f, 50.0f);
  GLfloat specularExponent = 50;
  glUniform3fv(glGetUniformLocation(phongShader, "lightSource"), 1, &lightSource.x);
  glUniform1fv(glGetUniformLocation(phongShader, "specularExponent"), 1, &specularExponent);

  glUniformMatrix4fv(glGetUniformLocation(phongShader, "projMatrix"), 1, GL_TRUE, camera->projectionMatrix.m);


  clock_t t;
  t = clock();


  generatePatch(0, 0, 255);
  //generatePatch(1, 0, 255);
  addGeneratedTerrain();
 

  for(int y = 0; y < 3; y++){
    for(int x = 0; x < 3; x++){
      printf("Adding patch @ %i, %i\n", x, y);
    }
  }
  t = clock() - t;
  printf ("Time to generate terrain: %f seconds)\n",((float)t)/CLOCKS_PER_SEC);


}

void World::drawTerrainVector(TerrainPatch* t){
  t->draw(camera->cameraMatrix);
}

void World::draw(){

  //addGeneratedTerrain();
  camera->update();

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  skybox->draw(camera->cameraMatrix);

  //for_each(terrainVector.begin(), terrainVector.end(), drawTerrainVector);
  for(int i = 0; i < terrainVector.size(); i++){
    terrainVector.at(i)->draw(camera->cameraMatrix);
  }   

}


World::~World(){
  delete camera;
  delete skybox;
  terrainVector.clear();
  delete patchGenerator;
}
