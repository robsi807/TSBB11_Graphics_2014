#version 150

in vec2 texCoord;
in vec3 exNormal;
in vec3 surf;

out vec4 outColor;

uniform vec3 lightDirection;
uniform float specularExponent;

uniform int evader;
uniform sampler2D evaderTexture;
uniform sampler2D chaserTexture;

uniform mat4 world2View;
uniform mat4 mdl2World;

void main(void)
{
	float shade,diffuseShade;
	vec3 reflectedLightDirection,eyeDirection,lightDir;
	vec3 normalizedNormal = normalize(exNormal);
	lightDir = normalize(mat3(world2View)*lightDirection); 

	float specularStrength = 0.0;
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

	if(evader == 1)
	   outColor = clamp(shade*texture(evaderTexture, texCoord),0,1);
	else
	   outColor = clamp(shade*texture(chaserTexture, texCoord),0,1);
}
