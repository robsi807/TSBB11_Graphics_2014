
#include "Camera.h"
//Constructor
Camera::Camera(vec3 pos, GLfloat vel, GLfloat sens, vector<vector<TerrainPatch*>> * terrain, int sizePatch, int overlap,int sizeGrid)
{
  vec3 r = vec3(0.5,0,0);
  position = pos;
  lookAtPoint = VectorAdd(position,r);
  upVector = vec3(0,1,0);
  
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
  projectionFar = 1700.0;
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
  flockIndex = -1; // Is set to zero first time we choose to follow a flock.
}
// Constructor for initializing frustum
/*Camera::Camera(float left, float right, float bottom, float top, float near, float far)
{
  vec3 r = SetVector(-0.5,0,-0.5);
  //position = SetVector(0,0,0);
  lookAtPoint = VectorAdd(position,r);
  upVector = SetVector(0,1,0);

  //cameraMatrix = lookAtv(position,lookAtPoint,upVector);
  velocity = 0.5;

  projectionLeft = left;
  projectionRight = right;
  projectionBottom = bottom;
  projectionTop = top;
  projectionNear = near;
  projectionFar = far;

  warpPointer = false;
}*/

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

  //y,g,h,j to move north,south,west and east, independent of direction
   if(keyIsDown('y'))
    {
      vec3 w = vec3(0.0,0.0,1.0);
      lookAtPoint = VectorAdd(lookAtPoint,ScalarMult(w,velocity));
      position = VectorAdd(position,ScalarMult(w,velocity));
    }  
  if(keyIsDown('h'))
    {
      vec3 s = vec3(0.0,0.0,-1.0);
      lookAtPoint = VectorAdd(lookAtPoint,ScalarMult(s,velocity));
      position = VectorAdd(position,ScalarMult(s,velocity));
    }
  if(keyIsDown('g'))
    {
      vec3 a = vec3(1.0,0.0,0.0);
      lookAtPoint = VectorAdd(lookAtPoint,ScalarMult(a,velocity));
      position = VectorAdd(position,ScalarMult(a,velocity));
    }
  if(keyIsDown('j'))
    {
      vec3 d = vec3(-1.0,0.0,0.0);
      lookAtPoint = VectorAdd(lookAtPoint,ScalarMult(d,velocity));
      position = VectorAdd(position,ScalarMult(d,velocity));
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
      velocity += 1.1;
    }
  if(keyIsDown('-'))
    {
      velocity /= 1.1;
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

  //f to print current position and direction
  if(keyIsDown('f'))
    {
      vec3 dir = getDirection();
      printf("Pos: (%3.1f,%3.1f,%3.1f)\n",position.x,position.y,position.z);
      printf("Dir: ((%1.2f,%1.2f,%1.2f))\n",dir.x,dir.y,dir.z);
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

  timer++;

  //cameraMatrix = lookAtv(position,lookAtPoint,upVector); // In update!

}

void Camera::handleMouse(int x, int y)
{
  GLfloat xtemp = (GLfloat)(-x + SCREEN_WIDTH / 2) / (10000 / sensitivity);
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
  glutWarpPointer(SCREEN_WIDTH/2, (SCREEN_HEIGHT/2)-520); // On mac the pointer is shifted 520 pixels (why?)
  glutHideCursor();
#else
  if(warpPointer){
    glutWarpPointer(SCREEN_WIDTH/2, SCREEN_HEIGHT/2); // Ger delay med linux!
  }
#endif
}

void Camera::update()
{

  // calc patch coordinate to adjust our height.

  float xPosition,zPosition;    
  if(position.x < 0) {
      xPosition = blendedSize-1*(fmod(-1*position.x,(float)(blendedSize)));
  }  
  else {
      xPosition = fmod(position.x,(float)(blendedSize));  
  }   
  if(position.z < 0) {
      zPosition = blendedSize-1*(fmod(-1*position.z,(float)(blendedSize)));
  } 
  else {
      zPosition = fmod(position.z,(float)(blendedSize));  
  }   

  int tempPatchXIndex = floor((position.x - xPosition)/(float)(blendedSize));
  int tempPatchZIndex = floor((position.z - zPosition)/(float)(blendedSize));

  int i,j;    
  if( tempPatchXIndex != actualPatchXIndex || tempPatchZIndex != actualPatchZIndex){     
    actualPatchXIndex = tempPatchXIndex;        
    actualPatchZIndex = tempPatchZIndex;        
    for(i = 0; i < gridSize; i++){
        if(terrainVector->at(i).at(0)->yGrid == actualPatchZIndex){
            actualPatchRow = terrainVector->at(i);
        }
    }
    for(i = 0; i < gridSize; i++){
        if(actualPatchRow.at(i)->xGrid == actualPatchXIndex){
            actualPatch = actualPatchRow.at(i);
        }
    }
  }
  float actualY = actualPatch->calcHeight(xPosition,zPosition);
  float yDiff = position.y - (actualY + groundOffset);
  if(!flying){
    position.y += -yDiff/2;
    lookAtPoint.y += -yDiff/2;
  }
  else{
    if(yDiff < 0){
    position.y += -yDiff/2;
    lookAtPoint.y += -yDiff/2;
    }
  }
      
  cameraMatrix = lookAtv(position,lookAtPoint,upVector);
  if(!lockFrustum)
    frustumPlanes->update(this);
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
