#include "Frustum.h"

Frustum::Frustum(Camera* cam){
  update(cam);
}

void Frustum::update(Camera* cam){
  mat4 m = Mult(cam->projectionMatrix,cam->cameraMatrix);
  //left plane
  planes[0].a = m.m[12]+m.m[0];
  planes[0].b = m.m[13]+m.m[1];
  planes[0].c = m.m[14]+m.m[2];
  planes[0].d = m.m[15]+m.m[3];
  //right plane
  planes[1].a = m.m[12]-m.m[0];
  planes[1].b = m.m[13]-m.m[1];
  planes[1].c = m.m[14]-m.m[2];
  planes[1].d = m.m[15]-m.m[3];
  //bottom plane
  planes[2].a = m.m[12]+m.m[4];
  planes[2].b = m.m[13]+m.m[5];
  planes[2].c = m.m[14]+m.m[6];
  planes[2].d = m.m[15]+m.m[7];
  //top plane
  planes[3].a = m.m[12]-m.m[4];
  planes[3].b = m.m[13]-m.m[5];
  planes[3].c = m.m[14]-m.m[6];
  planes[3].d = m.m[15]-m.m[7];
  //near plane
  planes[4].a = m.m[12]+m.m[8];
  planes[4].b = m.m[13]+m.m[9];
  planes[4].c = m.m[14]+m.m[10];
  planes[4].d = m.m[15]+m.m[11];
  //far plane
  planes[5].a = m.m[12]-m.m[8];
  planes[5].b = m.m[13]-m.m[9];
  planes[5].c = m.m[14]-m.m[10];
  planes[5].d = m.m[15]-m.m[11];

  planes[0].planeNormalize();
  planes[1].planeNormalize();
  planes[2].planeNormalize();
  planes[3].planeNormalize();
  planes[4].planeNormalize();
  planes[5].planeNormalize();
}
/*
// Deprecated version using sphere as bounding shape
bool Frustum::containsPatch(TerrainPatch* patch){
  vec3 patchPos = vec3(patch->xPos,0,patch->yPos);
  float patchWidth = patch->size;
  // Shifts position to middle of patch
  patchPos.x += patchWidth/2;
  patchPos.z += patchWidth/2;
  float patchRadius = patchWidth/2 * sqrt(2);
  bool found = true;
  int i = 0;
  while(i<6 && found == true)
    {
      if(planeClassifySphere(planes[i],patchPos,patchRadius) < 0)
	{
	  found = false;
	}
      i++;
    }
  return found;
}
*/

bool Frustum::containsPatch(TerrainPatch* patch){
  vec3 patchPos = vec3(patch->xPos,0.0,patch->yPos);
  float patchSize = patch->blendedSize;
  float blendedSize = patch->blendedSize + 1;
  patchPos.x += patchSize/2;
  patchPos.z += patchSize/2;
  float patchRadius = blendedSize/2 * sqrt(2);
  
  bool found = true;
  int i = 0;
  while(i<6 && found == true)
    {
      // Skip bottom and top plane
      if(i == 2)
	    i = 4;
      if(planeClassifyInfiniteCylinder(planes[i],patchPos,patchRadius) < 0)
	    {
	      found = false;
	    }
      i++;
    }
  return found;
}


bool Frustum::containsPlant(Plant* plant){
  vec3 plantPos = plant->globalPosition;
  float plantRadius = 6.0 * plant->scale;
  
  bool found = true;
  int i = 0;
  while(i<6 && found == true)
    {
      // Skip bottom and top plane
      if(i == 2)
	    i = 4;
      if(planeClassifyInfiniteCylinder(planes[i],plantPos,plantRadius) < 0)
	    {
	     found = false;
    	}
      i++;
    }
  return found;
}

// ------------------------ Plane functions -----------------------------
void Plane::planeNormalize()
{
  float magnitude = sqrt(a*a + b*b + c*c);
  a = a/magnitude;
  b = b/magnitude;
  c = c/magnitude;
  d = d/magnitude;
}

float planeDistanceToPoint(Plane plane, vec3 p){
  //printf("%f %f %f \n",p.x,p.y,p.z);
  return plane.a*p.x + plane.b*p.y + plane.c*p.z + plane.d;
}

vec3 planeProjectionOfPoint(Plane plane, vec3 p){
  vec3 res = vec3(.0,.0,.0);
  float a = plane.a;
  float b = plane.b;
  float c = plane.c;
  float d = plane.d;
  float u = p.x;
  float v = p.y; 
  float w = p.z; 
  float temp = (a*u + b*v + c*w +d);
  res.x = u - a * temp;
  res.y = v - b * temp;
  res.z = w - c * temp; 
  return res;
}

Halfspace planeClassifyPoint(Plane plane, vec3 p)
{
  float d = 0;
  d = planeDistanceToPoint(plane,p);
  //printf ("d %f \n",d);
  if(d < 0)
    return NEGATIVE;
  if(d > 0)
    return POSITIVE;
  return ON_PLANE;
}

Halfspace planeClassifySphere(Plane plane, vec3 pos, float radius)
{
  float d = 0;
  d = planeDistanceToPoint(plane,pos);
  //printf ("d %f \n",d);
  if(d < -radius)
    return NEGATIVE;
  if(d > -radius)
    return POSITIVE;
  return ON_PLANE;
}

// Infinite cylinder along y-axis
Halfspace planeClassifyInfiniteCylinder(Plane plane, vec3 pos, float radius)
{
  // Check if center position is on positive side
  float d = planeDistanceToPoint(plane,pos);
  if(d > 0){
    return POSITIVE;
  }
  else {
    // Check if outmost point on cylinder is on positive side
    vec3 posProj = planeProjectionOfPoint(plane,pos);
    float dProj = Norm(posProj - pos);

    if(dProj < radius)
      return POSITIVE;
    else if(dProj > radius)
      return NEGATIVE;
    else 
      return ON_PLANE;
  }
}


