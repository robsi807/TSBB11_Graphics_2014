#version 150

in vec3 texCoord;
in vec3 skyboxColor;
uniform float time;
uniform samplerCube cubeMap;

out vec4 out_Color;

void main(void)
{
	float nightVar = (1.0 - cos(time))/2.0;
	
	//out_Color = 0.5*(1.0 - nightVar)*texture(cubeMap, texCoord) + nightVar*vec4(skyboxColor,1.0);

	out_Color = 0.5*nightVar*texture(cubeMap, texCoord) + vec4(skyboxColor,1.0); 

}
