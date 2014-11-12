#version 150

in vec2 texCoordG;
in vec3 exNormalG;
in vec3 surf;
in vec3 exPosition;

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
	vec3 normalizedNormal = normalize(exNormalG);
	lightDir = normalize(mat3(world2View)*lightDirection); 

	float specularStrength = 0.0;
	diffuseShade = max(dot(normalizedNormal, lightDir),0.01);

	if(dot(normalizedNormal,lightDir) > 0.0)
	{
		reflectedLightDirection = reflect(normalize(-lightDir),normalize(exNormalG));
		eyeDirection = -normalize(surf);

		specularStrength = dot(reflectedLightDirection, eyeDirection);
		specularStrength = max(specularStrength, 0.01);
		specularStrength = pow(specularStrength, specularExponent);
	}

	shade = (0.7*diffuseShade + 0.3*specularStrength + 0.000001*texCoordG.s);

	outColor = clamp(vec4(shade), 0,1);
	//outColor = clamp(shade*texture(tex, texCoordG),0,1);
	//outColor = clamp(vec4(lightSource,1)*vec4(exNormalG,1)*vec4(surf,1)*texture(tex, texCoordG),0,1);
}
