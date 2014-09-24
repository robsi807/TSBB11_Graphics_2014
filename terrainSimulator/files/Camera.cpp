
#include "Camera.h"

Camera::Camera()
{
  vec3 r = SetVector(-0.5,0,-0.5);
  //camPosition = SetVector(0,0,0);
  lookAtPoint = VectorAdd(camPosition,r);
  //upVector = SetVector(0,1,0);

  camMatrix = lookAtv(camPosition,lookAtPoint,upVector);
  velocity = 0.5;
}

void Camera::init(vec3 pos, int width, int height, GLfloat vel, GLfloat sens)
{
  initKeymapManager();
  vec3 r = SetVector(-0.5,0,-0.5);
  camPosition = pos; //SetVector(24,5,24);
  lookAtPoint = VectorAdd(pos,r);
  
  camMatrix = lookAtv(camPosition,lookAtPoint,upVector);
  velocity = vel;
  sensitivity = sens;
  windowWidth = width;
  windowHeight = height;
}


void Camera::handleKeyPress()
{
  if(keyIsDown('w'))
  {
    vec3 w = Normalize(VectorSub(lookAtPoint,camPosition));
    lookAtPoint = VectorAdd(lookAtPoint,ScalarMult(w,velocity));
    camPosition = VectorAdd(camPosition,ScalarMult(w,velocity));
  }
  if(keyIsDown('s'))
  {
    vec3 s = Normalize(VectorSub(camPosition,lookAtPoint));
    lookAtPoint = VectorAdd(lookAtPoint,ScalarMult(s,velocity));
    camPosition = VectorAdd(camPosition,ScalarMult(s,velocity));
  }
  if(keyIsDown('a'))
  {
    vec3 w = VectorSub(lookAtPoint,camPosition);
    vec3 a = Normalize(CrossProduct(upVector,w));
    lookAtPoint = VectorAdd(lookAtPoint,ScalarMult(a,velocity));
    camPosition = VectorAdd(camPosition,ScalarMult(a,velocity));
  }
  if(keyIsDown('d'))
  {
    vec3 w = VectorSub(lookAtPoint,camPosition);
    vec3 d = Normalize(CrossProduct(w,upVector));
    lookAtPoint = VectorAdd(lookAtPoint,ScalarMult(d,velocity));
    camPosition = VectorAdd(camPosition,ScalarMult(d,velocity));
  }

//camMatrix = lookAtv(camPosition,lookAtPoint,upVector); // In update!

}

void Camera::handleMouse(int x, int y)
{
  GLfloat xtemp = (GLfloat)(-x + windowWidth / 2) / (10000 / sensitivity);
  GLfloat ytemp = (GLfloat)(-y + windowHeight / 2) / (10000 / sensitivity);
  vec3 r = Normalize(VectorSub(lookAtPoint, camPosition)); // Forward Direction
  vec3 d = Normalize(CrossProduct(r, upVector)); // Right direction
  mat4 phiRot = ArbRotate(upVector, xtemp);
  r = MultVec3(phiRot, r);
  mat4 thetaRot = ArbRotate(d, ytemp);
  r = MultVec3(thetaRot, r);
  //upVector = MultVec3(thetaRot,upVector); // for flight simulator
  lookAtPoint = VectorAdd(camPosition, r);
  //camMatrix = lookAtv(camPosition,lookAtPoint,upVector); // In update!

#ifdef __APPLE__
  glutWarpPointer(windowWidth/2, (windowHeight/2)-520); // On mac the pointer is shifted 520 pixels (why?)
#else
  glutWarpPointer(windowWidth/2, windowHeight/2);
#endif
  
  glutHideCursor();
}

void Camera::update()
{
  camMatrix = lookAtv(camPosition,lookAtPoint,upVector);
}
