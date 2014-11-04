#version 150

in vec3 texCoord;
uniform samplerCube cubeMap;

out vec4 out_Color;

void main(void)
{
	out_Color = texture(cubeMap, texCoord);
}
