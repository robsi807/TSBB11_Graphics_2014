#version 150

in vec2 texCoord;
in vec3 exNormal;
in vec3 surf;
//in vec4 exPosition;

out vec4 outColor;

uniform vec3 lightDirection;
uniform float specularExponent;
uniform sampler2D tex;
uniform mat4 world2View;
uniform mat4 mdl2World;

void main(void)
{


	
	float shade,diffuseShade;
	vec3 reflectedLightDirection,eyeDirection,lightDir;
	vec3 normalizedNormal = normalize(exNormal);
	lightDir = normalize(mat3(world2View)*lightDirection); 

	float specularStrength = 0.0;
	
	float texScale = 512;
	
	diffuseShade = max(dot(normalizedNormal, lightDir),0.01);

	if(dot(normalizedNormal,lightDir) > 0.0)
	{
		reflectedLightDirection = reflect(normalize(-lightDir),normalize(exNormal));
		eyeDirection = -normalize(surf);

		specularStrength = dot(reflectedLightDirection, eyeDirection);
		specularStrength = max(specularStrength, 0.01);
		specularStrength = pow(specularStrength, specularExponent);
	}

	shade = 0.7*diffuseShade + 0.3*specularStrength;

  //texCoord = texCoord + 0.25;


	//outColor = clamp(vec4(shade), 0,1);
	outColor = clamp(shade*texture(tex,((texCoord)/texScale)),0,1);
	//outColor = clamp(vec4(lightSource,1)*vec4(exNormalG,1)*vec4(surf,1)*texture(tex, texCoordG),0,1);
}
