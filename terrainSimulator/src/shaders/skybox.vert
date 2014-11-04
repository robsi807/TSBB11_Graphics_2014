#version 150

in vec3 vertices;

uniform mat4 projMatrix;
uniform mat4 mdl2World;
uniform mat4 world2View;

out vec3 texCoord;

void main(void)
{
	gl_Position = projMatrix*world2View*mdl2World*vec4(vertices, 1.0);
	texCoord = vertices;
}
