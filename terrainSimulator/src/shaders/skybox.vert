#version 150

in vec3 vertices;

uniform mat4 projMatrix;
uniform mat4 mdl2World;
uniform mat4 world2View;

uniform vec3 bottomColor;
uniform vec3 topColor;

out vec3 texCoord;
out vec3 skyboxColor;

void main(void)
{    
	 //vec3 bottomColor = vec3(1.0, 1.0, 1.0);
	 //vec3 topColor = vec3(0.6, 0.87, 0.99);
     
     gl_Position = projMatrix*world2View*mdl2World*vec4(vertices, 1.0);
     texCoord = vertices;

     if(vertices.y < 0)
      skyboxColor = bottomColor;
     else
      skyboxColor = topColor;
}
