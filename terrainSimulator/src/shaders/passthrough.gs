#version 150

layout(triangles) in;
layout(triangle_strip, max_vertices = 12) out;

in vec2 texCoord[3];  
in vec3 exNormal[3];
out vec2 texCoordG;
out vec3 exNormalG;

uniform mat4 projMatrix;
uniform mat4 mdl2World;
uniform mat4 world2View;

void main()
{
  for(int i = 0; i < gl_in.length(); i++)
  {


      gl_Position = gl_in[i].gl_Position;
      texCoordG = texCoord[i];
      exNormalG = exNormal[i];
      gl_Position = projMatrix * world2View * mdl2World * gl_Position;
      EmitVertex();


      gl_Position = gl_in[i].gl_Position;
      gl_Position.y += 1;
      texCoordG = texCoord[i];
      exNormalG = exNormal[i];
      gl_Position = projMatrix * world2View * mdl2World * gl_Position;
      EmitVertex();

      if(i+1 < 3){
        gl_Position = gl_in[i+1].gl_Position;
        texCoordG = texCoord[i+1];
        exNormalG = exNormal[i+1];
        gl_Position = projMatrix * world2View * mdl2World * gl_Position;
        EmitVertex();

        gl_Position = gl_in[i+1].gl_Position;
        gl_Position.y += 1;
        texCoordG = texCoord[i+1];
        exNormalG = exNormal[i+1];
        gl_Position = projMatrix * world2View * mdl2World * gl_Position;
        EmitVertex();

      }else{
        gl_Position = gl_in[0].gl_Position;
        texCoordG = texCoord[0];
        exNormalG = exNormal[0];
        gl_Position = projMatrix * world2View * mdl2World * gl_Position;
        EmitVertex();

        gl_Position = gl_in[0].gl_Position;
        gl_Position.y += 1;
        texCoordG = texCoord[0];
        exNormalG = exNormal[0];
        gl_Position = projMatrix * world2View * mdl2World * gl_Position;
        EmitVertex();

      } 


      EndPrimitive();

  }

}

