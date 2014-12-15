
#include "Camera.h"

//Constructor
Camera::Camera(vec3 pos, GLfloat vel, GLfloat sens, vector<vector<TerrainPatch*>> * terrain, int sizePatch, int overlap,int sizeGrid)
{
  vec3 r = vec3(0.5,0,0);
  position = pos;
  lookAtPoint = VectorAdd(position,r);
  upVector = vec3(0.0,1.0,0.0);
  
  //Terrain information
  terrainVector = terrain;
  patchSize = sizePatch;
  patchOverlap = overlap;
  blendedSize = patchSize-patchOverlap;
  gridSize = sizeGrid;
  //Set inital patch not to equal to starting patch for comparison in  
  actualPatchXIndex = 100;
  actualPatchZIndex = 100;
  groundOffset = 10;
  flying = false;


  //cameraMatrix = lookAtv(position,lookAtPoint,upVector);
  velocity = 1.5;

  projectionNear = 0.8;
#if LOWGRAPHICS == 1
  projectionFar = 800.0;  
#else 
  projectionFar = 1700.0;
#endif
  projectionRight = 0.5/0.75; // for wide screen
  projectionLeft = -0.5/0.75; // for wide screen
  projectionTop = 0.5;
  projectionBottom = -0.5;

  warpPointer=true;
  lockFrustum=false;
  initKeymapManager();

  cameraMatrix = lookAtv(position,lookAtPoint,upVector);
  velocity = vel;
  sensitivity = sens;

  projectionMatrix = frustum(projectionLeft, projectionRight, projectionBottom, projectionTop,projectionNear, projectionFar);

  frustumPlanes = new Frustum(this);

  timer = 30;
  followFlock = false;
  birdView = true;
  flockIndex = -1; // Is set to zero first time we choose to follow a flock.
}

//Keyhandler class to move araound in the world.
void Camera::handleKeyPress()
{
  //w,a,s,d to move forward,backwards and sideways
  if(keyIsDown('w'))
    {
      vec3 w = Normalize(VectorSub(lookAtPoint,position));
      lookAtPoint = VectorAdd(lookAtPoint,ScalarMult(w,velocity));
      position = VectorAdd(position,ScalarMult(w,velocity));
    }  
  if(keyIsDown('s'))
    {
      vec3 s = Normalize(VectorSub(position,lookAtPoint));
      lookAtPoint = VectorAdd(lookAtPoint,ScalarMult(s,velocity));
      position = VectorAdd(position,ScalarMult(s,velocity));
    }
  if(keyIsDown('a'))
    {
      vec3 w = VectorSub(lookAtPoint,position);
      vec3 a = Normalize(CrossProduct(upVector,w));
      lookAtPoint = VectorAdd(lookAtPoint,ScalarMult(a,velocity));
      position = VectorAdd(position,ScalarMult(a,velocity));
    }
  if(keyIsDown('d'))
    {
      vec3 w = VectorSub(lookAtPoint,position);
      vec3 d = Normalize(CrossProduct(w,upVector));
      lookAtPoint = VectorAdd(lookAtPoint,ScalarMult(d,velocity));
      position = VectorAdd(position,ScalarMult(d,velocity));
    }
  //y,h,g,j to rotate camera up, left, down right
  if(keyIsDown('y'))
    {
      vec3 w = VectorSub(lookAtPoint,position);
      vec3 a = Normalize(VectorAdd(w,ScalarMult(upVector,1.0/10.0)));
      lookAtPoint = VectorAdd(position,a);
    }  
  if(keyIsDown('h'))
    {
      vec3 w = VectorSub(lookAtPoint,position);
      vec3 a = Normalize(VectorAdd(w,ScalarMult(upVector,-1.0/10.0)));
      lookAtPoint = VectorAdd(position,a);
    }
  if(keyIsDown('g'))
    {
      vec3 w = VectorSub(lookAtPoint,position);
      vec3 a = Normalize(CrossProduct(upVector,w));
      a = Normalize(VectorAdd(w,ScalarMult(a,1.0/10.0)));
      lookAtPoint = VectorAdd(position,a);
    }
  if(keyIsDown('j'))
    {
      vec3 w = VectorSub(lookAtPoint,position);
      vec3 a = Normalize(CrossProduct(upVector,w));
      a = Normalize(VectorAdd(w,ScalarMult(a,-1.0/10.0)));
      lookAtPoint = VectorAdd(position,a);
    }
  //space and c to increase or decrease height
  if(keyIsDown(' '))
    {
      vec3 w = vec3(0,1,0);
      lookAtPoint = VectorAdd(lookAtPoint,ScalarMult(w,velocity));
      position = VectorAdd(position,ScalarMult(w,velocity));
    }
  if(keyIsDown('c'))
    {
      vec3 w = vec3(0,-1,0);
      lookAtPoint = VectorAdd(lookAtPoint,ScalarMult(w,velocity));
      position = VectorAdd(position,ScalarMult(w,velocity));
    }
  //p and o to toggle warp pointer on and off
  if(keyIsDown('p'))
    {
      warpPointer = true;
    }
  if(keyIsDown('P'))
    {
      warpPointer = false;
    }
  //m and n to toggle flight mode on and off
  if(keyIsDown('m'))
    {
      flying = true;
    }
  if(keyIsDown('M') && !followFlock)
    {
      flying = false;
    }
  //+ and - to increase or decrease movement speed
  if(keyIsDown('+'))
    {
      velocity = min(3.0,velocity + 1.0);
    }
  if(keyIsDown('-'))
    {
      velocity = max(1.0,velocity - 1.0);
    }

  //1 and 3 to lock or unlock frustum, for debugging purposes
  if(keyIsDown('1'))
    {
      lockFrustum = true;
    }
  if(keyIsDown('!'))
    {
      lockFrustum = false;
    }
  if(keyIsDown('o') && timer > 20 && flying)
    {
      timer = 0;
      followFlock = true;
      flockIndex++; // Follow next flock each time 'o' is pressed.
    }
  if(keyIsDown('O') && followFlock)
    {
      followFlock = false;
      flockIndex--; // Next time 'o' is pressed, we will follow the previous followed flock.
    }
  if(keyIsDown('0') && birdView && followFlock)
    {
      birdView = false;
    }
  if(keyIsDown('=') && !birdView && followFlock)
    {
      birdView = true;
    }

  timer++;
}

void Camera::handleMouse(int x, int y)
{
  /*GLfloat xtemp = (GLfloat)(-x + SCREEN_WIDTH / 2) / (10000 / sensitivity);
    GLfloat ytemp = (GLfloat)(-y + SCREEN_HEIGHT / 2) / (10000 / sensitivity);

    vec3 r = Normalize(VectorSub(lookAtPoint, position)); // Forward Direction
    vec3 d = Normalize(CrossProduct(r, upVector)); // Right direction
    mat4 phiRot = ArbRotate(upVector, xtemp);
    r = MultVec3(phiRot, r);
    mat4 thetaRot = ArbRotate(d, ytemp);
    r = MultVec3(thetaRot, r);

    vec3 temp = CrossProduct(r, upVector);
    //std::cout << "temp.x = " << std::abs(temp.x) << std::endl;
    //std::cout << "temp.y = " << std::abs(temp.y) << std::endl;
    //std::cout << "temp.z = " << std::abs(temp.z) << std::endl;
    if(std::abs(temp.x)  > 0.09 || std::abs(temp.y) > 0.09 || std::abs(temp.z) > 0.09) // Still possible to get 180 degree flip! But much better.
    lookAtPoint = VectorAdd(position, r);

    //upVector = MultVec3(thetaRot,upVector); // for flight simulator
    //cameraMatrix = lookAtv(position,lookAtPoint,upVector); // In update!

    #ifdef __APPLE__
    #else

    #endif*/
}

void Camera::update()
{
  // Calc. patch coordinate to adjust our height.

  float xPosition,zPosition;    
  if(position.x < 0.0) {
    xPosition = blendedSize-1*(fmod(-1*position.x,(float)(blendedSize)));
  }
  else {
    xPosition = fmod(position.x,(float)(blendedSize));  
  }
  if(position.z < 0.0) {
    zPosition = blendedSize-1*(fmod(-1*position.z,(float)(blendedSize)));
  }
  else {
    zPosition = fmod(position.z,(float)(blendedSize));  
  }

  int tempPatchXIndex = floor((position.x - xPosition)/(float)(blendedSize));
  int tempPatchZIndex = floor((position.z - zPosition)/(float)(blendedSize));

  if(tempPatchXIndex != actualPatchXIndex || tempPatchZIndex != actualPatchZIndex){     
    actualPatchXIndex = tempPatchXIndex;        
    actualPatchZIndex = tempPatchZIndex;        
    for(int i = 0; i < gridSize; i++){
      if(terrainVector->at(i).at(0)->yGrid == actualPatchZIndex){
	actualPatchRow = terrainVector->at(i);
      }
    }
    for(int i = 0; i < gridSize; i++){
      if(actualPatchRow.at(i)->xGrid == actualPatchXIndex){
	actualPatch = actualPatchRow.at(i);
      }
    }
  }
  float actualY = actualPatch->calcHeight(xPosition,zPosition);
  float yDiff = position.y - (actualY + groundOffset);
  if(!flying){
    position.y += -yDiff/2.0;
    lookAtPoint.y += -yDiff/2.0;
  }
  else{
    if(yDiff < 0.0){
      position.y += -yDiff/2.0;
      lookAtPoint.y += -yDiff/2.0;
    }
  }
      
  cameraMatrix = lookAtv(position,lookAtPoint,upVector);
  if(!lockFrustum)
    frustumPlanes->update(this);
}

float Camera::getActualHeight(vec3 objectPos)
{
  float actualY;
  vec3 tempObjectPos = objectPos;
  vec3 tempCameraPos = position;
  tempObjectPos.y = 0;
  tempCameraPos.y = 0;
  // (gridSize-2)*patchSize is the maximum distance, -3 for safety.
  if(Norm(tempCameraPos - tempCameraPos) < (gridSize-3)*patchSize)
    {
      float xPosition,zPosition;    
      if(objectPos.x < 0.0) {
	xPosition = blendedSize-1*(fmod(-1*objectPos.x,(float)(blendedSize)));
      }
      else {
	xPosition = fmod(objectPos.x,(float)(blendedSize));  
      }
      if(objectPos.z < 0.0) {
	zPosition = blendedSize-1*(fmod(-1*objectPos.z,(float)(blendedSize)));
      }
      else {
	zPosition = fmod(objectPos.z,(float)(blendedSize));  
      }

      std::vector<TerrainPatch*> objectActualPatchRow;
      TerrainPatch *objectActualPatch;

      int patchXIndex = floor((objectPos.x - xPosition)/(float)(blendedSize));
      int patchZIndex = floor((objectPos.z - zPosition)/(float)(blendedSize));
        
      for(int i = 0; i < gridSize; i++){
	if(terrainVector->at(i).at(0)->yGrid == patchZIndex){
	  objectActualPatchRow = terrainVector->at(i);
	}
      }
      // Can trig out of range in objectActualPatchRow and probably
      // earlier in terrainVector also.
      for(int i = 0; i < gridSize; i++){
	if(objectActualPatchRow.at(i)->xGrid == patchXIndex){
	  objectActualPatch = objectActualPatchRow.at(i);
	}
      }

      actualY = objectActualPatch->calcHeight(xPosition,zPosition);
    }
  else
    actualY = 90.0;
  
  return actualY;
}

vec3 Camera::getDirection(){
  return VectorSub(lookAtPoint, position);
}

vec3 Camera::getPosition(){
  return position;
}

//Check if a patch is inside the frustum
bool Camera::isInFrustum(TerrainPatch* patch){
  return frustumPlanes->containsPatch(patch);
}

//Check if a plant is inside the frustum
bool Camera::isInFrustum(Plant* plant){
  return frustumPlanes->containsPlant(plant);
}

//Check if a boid is inside the frustum
bool Camera::isInFrustum(Boid* boid){
  return frustumPlanes->containsBoid(boid);
}
