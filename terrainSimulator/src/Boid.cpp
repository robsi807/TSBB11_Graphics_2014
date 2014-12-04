
#include "Boid.h"
Boid::Boid()
{
  //Construtor for leader. Maybe make a class of it because it does not need all that a boid needs
  position = vec3(0,0,0);
  averagePosition = vec3(0.0, 0.0, 0.0);
  
  speed = vec3(0,0,0); //vec3(1.0, 0.2, 0.8);
  speedDifference = vec3(0.0, 0.0, 0.0);
  
  avoidanceVector = vec3(0.0, 0.0, 0.0);
  cohesionVector = vec3(0.0, 0.0, 0.0);
  alignmentVector = vec3(0.0, 0.0, 0.0);

  forward = vec3(0,0,-1); //Or vec3(0,0,1)?; According to blender: -1 in z
  up = vec3(0,1,0);
  //side = vec3(1,0,0);, needed?

  rotationMatrix = IdentityMatrix();
}

Boid::Boid(vec3 pos)
{
  position = pos;
  averagePosition = vec3(0.0, 0.0, 0.0);
  
  speed = vec3(0,0,0); //vec3(1.0, 0.2, 0.8);
  speedDifference = vec3(0.0, 0.0, 0.0);
  
  avoidanceVector = vec3(0.0, 0.0, 0.0);
  cohesionVector = vec3(0.0, 0.0, 0.0);
  alignmentVector = vec3(0.0, 0.0, 0.0);

  forward = vec3(0,0,-1); //Or vec3(0,0,1)?;
  up = vec3(0,1,0);
  //side = vec3(1,0,0);, needed?

  rotationMatrix = IdentityMatrix();

  uint max = 40;
  uint min = 0;
  animationIndex = rand()%(max-min + 1) + min;
}

// void Boid::setDirection()
// {

//   //vec3 p = vec3(averagePosition.x*50, averagePosition.y, averagePosition.z*50);
//   //direction = position - averagePosition;

// }

// If the birds are flying side by side it looks weird if all of them looks att the same point
void Boid::setRotation()
{
  // vec3 vn,vp;
  //splitVector(direction,up,&vn,&vp);

  vec3 v = forward;
  vec3 v2 = Normalize(direction); //Normalize(vec3(direction.x, 0.0, direction.z));

  //std::cout << "Direction for boid i: (" << direction.x << "," << direction.y << "," << direction.z << ")" << std::endl;
  
  //float theta = acos(DotProduct(v2,v)/(Norm(v2)*Norm(v)));
  float theta = 3.14 - atan2((v2.x-v.x),(v2.z-v.z));
  //std::cout << "Theta = " << theta << std::endl;

  rotationMatrix = Ry(theta);

  forward = direction;
  //rotationMatrix = ArbRotate(direction, theta); //Mult(yRot,xRot);

}

/*Boid::~Boid()
{

}*/

void Boid::draw(mat4 cameraMatrix, GLuint* shader, Model* model, GLuint* texture)
{
  mat4 translationMatrix = T(position.x, position.y, position.z);
  mat4 transformationMatrix = Mult(translationMatrix, rotationMatrix);
  glUseProgram(*shader);
  glUniformMatrix4fv(glGetUniformLocation(*shader, "mdl2World"), 1, GL_TRUE, transformationMatrix.m);
  glUniformMatrix4fv(glGetUniformLocation(*shader, "world2View"), 1, GL_TRUE, cameraMatrix.m);
  glBindTexture(GL_TEXTURE_2D, *texture);	
  DrawModel(model, *shader, "inPosition", "inNormal", "inTexCoord");
}

void Boid::animate(GLfloat time)
{
  float xSpeed = cos(time);
  float ySpeed = 2*sin(time);
  float zSpeed = sin(time);

  //speed = vec3(xSpeed,ySpeed,zSpeed);

  //position += speed;
}
