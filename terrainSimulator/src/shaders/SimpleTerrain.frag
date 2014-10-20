#version 150

// terrain.frag
// Author: Peter Thulin

in vec2 texCoord;
in vec3 exNormal;
in vec3 exPosition;
in vec3 surf;
in float cameraDist;

out vec4 outColor;

uniform vec3 lightSource;
uniform float specularExponent;
uniform sampler2D tex;

/*
// A function for procedural coloring of surfaces
vec3 calculateColor(vec3 normal,vec3 position)
{
	const vec3 snow = vec3(0.8,0.8,1.0);
	const vec3 rock = vec3(0.5,0.5,0.5);
	const vec3 grass = vec3(0.2,0.8,0.2);
	const vec3 sand = vec3(0.8,0.6,0.2);
	
	// Calculate normalized height
	const float heightScale = 0.0025; // TODO: this should be uploaded to the shader
	float normalizedHeight = position.y * heightScale;

	// Calculate slope
	float slope = clamp(1.1-normal.y,0,1); // Shortcut for dot product with y-axis!
	
	// Calculate the height color
	// TODO: Remove if statements and somehow weight
	
	vec3 heightColor = grass;
	
	if(normalizedHeight < 0.2)
	{
		heightColor = sand;	
	}
	if(normalizedHeight > 0.37)
	{
		heightColor = snow;
		slope = 0.1;
	}
	
	//heightColor = vec4(0,normalizedHeight,0,1.0);
	//float noise1 = 0.0; //cnoise(position);
	//float noiseTot = 0.0; //noise1;
	//vec3 noiseVec = 0.15*vec3(noiseTot,noiseTot,noiseTot);
	return slope*rock + (1-slope)*heightColor; // + noiseVec;
}
*/

// Simply fades to gray (or blue-ish)
vec3 applyDistanceFog(vec3 rgb){
     // fogColor should ideally be calculate from the skybox
     const vec3 fogColor = vec3(.8,.8,0.95);
     float fogAmount = clamp(cameraDist*0.002,0.0,1.0);
     return mix(rgb,fogColor,fogAmount);
}

void main(void)
{

	float shade,diffuseShade;
	vec3 reflectedLightDirection,eyeDirection,lightDir;

	lightDir = normalize(lightSource);
	
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
	shade = (0.7*diffuseShade + 0.4*specularStrength) + 0.0001*texCoord.s;
	//vec3 color = shade*vec3(shade,shade,shade);
	vec3 color = applyDistanceFog(vec3(0.5*shade,shade,0.5*shade));
	outColor = vec4(clamp(color, 0,1),1.0);
	//outColor = clamp(shade*texture(tex, texCoord),0,1);
	//outColor = clamp(vec4(lightSource,1)*vec4(exNormal,1)*vec4(surf,1)*texture(tex, texCoord),0,1);
}
