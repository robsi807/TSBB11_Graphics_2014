#version 150

layout(triangles) in;
layout(triangle_strip, max_vertices = 12) out;

in vec2 texCoord[3];  
in vec3 exNormal[3];
out vec2 texCoordG;
out vec3 exNormalG;


void main()
{
  for(int i = 0; i < 1; i++)
    {

 

    if(gl_in[i].gl_Position.z < 100){
    gl_Position = gl_in[i].gl_Position;
    gl_Position.y += 5+i;
    texCoordG = texCoord[i];
    exNormalG = exNormal[i];
    EmitVertex();
    
    gl_Position = gl_in[i].gl_Position;
    gl_Position.y += 5+i;
        gl_Position.x += 1;
    texCoordG = texCoord[i];
    exNormalG = exNormal[i];
    EmitVertex();
    
    gl_Position = gl_in[i].gl_Position;
    texCoordG = texCoord[i];
    exNormalG = exNormal[i];
    EmitVertex();
    
        gl_Position = gl_in[i].gl_Position;
        gl_Position.x += 1+i;
    texCoordG = texCoord[i];
    exNormalG = exNormal[i];
    EmitVertex();
    }
  EndPrimitive();
  }
}
