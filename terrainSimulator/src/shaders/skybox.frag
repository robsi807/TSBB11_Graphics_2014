#version 150

in vec3 texCoord;
in vec3 skyboxColor;
uniform samplerCube cubeMap;

out vec4 out_Color;

void main(void)
{
	float night = 1.0-abs(skyboxColor);
	//out_Color = texture(cubeMap, texCoord);
	out_Color =  night*texture(cubeMap, texCoord) + 0.7*vec4(skyboxColor, 1.0);
}
