#version 150

in vec2 texCoord;
in vec3 exNormal;
in vec3 surf;
in vec3 exPosition;

out vec4 outColor;

uniform vec3 lightSource;
uniform float specularExponent;
uniform sampler2D tex;
uniform mat4 world2View;
uniform mat4 mdl2World;

void main(void)
{

	float shade,diffuseShade;
	vec3 reflectedLightDirection,eyeDirection,lightDir;

	lightDir = normalize(vec3(world2View*vec4(lightSource,1)-vec4(exPosition,1)));
	
	float specularStrength = 0.0;
	diffuseShade = max(dot(normalize(exNormal), lightDir),0.01);

	if(dot(lightDir,exNormal) > 0.0)
	{
		reflectedLightDirection = reflect(normalize(lightDir),normalize(exNormal));
		eyeDirection = -normalize(surf);

		specularStrength = dot(reflectedLightDirection, eyeDirection);
		specularStrength = max(specularStrength, 0.01);
		specularStrength = pow(specularStrength, specularExponent);
	}

	shade = (0.7*diffuseShade + 0.4*specularStrength);

	outColor = clamp(shade*texture(tex, texCoord),0,1);
	//outColor = clamp(vec4(lightSource,1)*vec4(exNormal,1)*vec4(surf,1)*texture(tex, texCoord),0,1);
}
