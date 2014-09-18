
void handleKeyPress()
{
  // --- HANDLE KEY INPUT ----
  if(keyIsDown('w'))
  {
    vec3 w = Normalize(VectorSub(l,p));
    lookAtPoint= VectorAdd(l,ScalarMult(w,constSpeed));
    camPosition = VectorAdd(p,ScalarMult(w,constSpeed));
  }
  if(keyIsDown('s'))
  {
    vec3 s = Normalize(VectorSub(p,l));
    lookAtPoint= VectorAdd(l,ScalarMult(s,constSpeed));
    camPosition = VectorAdd(p,ScalarMult(s,constSpeed));
  }
  if(keyIsDown('a'))
  {
    vec3 w = VectorSub(l,p);
    vec3 a = Normalize(CrossProduct(v,w));
    lookAtPoint= VectorAdd(l,ScalarMult(a,constSpeed));
    camPosition = VectorAdd(p,ScalarMult(a,constSpeed));
  }
  if(keyIsDown('d'))
  {
    vec3 w = VectorSub(l,p);
    vec3 d = Normalize(CrossProduct(w,v));
    lookAtPoint= VectorAdd(l,ScalarMult(d,constSpeed));
    camPosition = VectorAdd(p,ScalarMult(d,constSpeed));
  }


  camMatrix = lookAtv(cam,lookAtPoint,upVector);

}

void handleMouse(int x,int y)
{
  GLfloat xtemcamPosition = (GLfloat)(-x+WIDTH/2)/20000;
  GLfloat ytemcamPosition = (GLfloat)(-y+HEIGHT/2)/20000;
  vec3 r = Normalize(VectorSub(l,p)); // Forward Direction
  vec3 d = Normalize(CrossProduct(r,v)); // Right direction
  mat4 phiRot = ArbRotate(v,xtemp);
  r = MultVec3(phiRot,r);
  mat4 thetaRot = ArbRotate(d,ytemp);
  r = MultVec3(thetaRot,r);
  //v = MultVec3(thetaRot,v); // for flight simulator
  lookAtPoint= VectorAdd(p,r);
  //int windowWidth = WIDTH;
  //int windowHeight = HEIGHT;
  //glutWarpPointer(windowWidth,windowHeight);
}
