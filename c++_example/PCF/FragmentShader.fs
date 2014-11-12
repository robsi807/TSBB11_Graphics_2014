#version 150

// Projected texture demo
// Based on a demo by Fabien Sanglard, http://www.fabiensanglard.net/shadowmapping/index.php

uniform sampler2D textureUnit;
in vec4 lightSourceCoord;
out vec4 out_Color;
uniform float shade;

float shadowContribWithOffset()
{
	return 0;
}

void main()
{	
	// Perform perspective division to get the actual texture position
	vec4 shadowCoordinateWdivide = lightSourceCoord / lightSourceCoord.w;
	
	// Used to lower moire' pattern and self-shadowing
	// The optimal value here will vary with different GPU's depending on their Z buffer resolution.
	shadowCoordinateWdivide.z -= 0.002; // 0.0005;

    float offset = 3.5;
    float divide = 64;
	float shadow = 1.0;
	float dx, dy;
	for (dy = -offset ; dy <=offset ; dy+=1.0)
		for (dx = -offset ; dx <=offset ; dx+=1.0)
		{
			float distanceFromLight = texture(textureUnit, shadowCoordinateWdivide.st + vec2(dx, dy)/250.0).x;
			distanceFromLight = (distanceFromLight-0.5) * 2.0;
			
			if (lightSourceCoord.w > 0.0)
				if (distanceFromLight < shadowCoordinateWdivide.z) // shadow
					shadow -= 0.5/divide; // = 0.5 shadow if total shadow (for 64 samples)
		}
		out_Color = vec4(shadow * shade);

/*
	Original, 1 sample:

	// Look up the depth value
	float distanceFromLight = texture(textureUnit, shadowCoordinateWdivide.st).x;
	distanceFromLight = (distanceFromLight-0.5) * 2.0;
	
	// Compare
	float shadow = 1.0; // No shadow

	if (lightSourceCoord.w > 0.0)
		if (distanceFromLight < shadowCoordinateWdivide.z) // shadow
			shadow = 0.5;
	out_Color = vec4(shadow * shade);
*/

// Debugging - other data mapped on scene
//	out_Color =	 vec4(distanceFromLight);
//	out_Color =	 vec4(shadowCoordinateWdivide.z);
//	out_Color =	 vec4(distanceFromLight - shadowCoordinateWdivide.z);
//	out_Color =	 vec4((shadowCoordinateWdivide.z - distanceFromLight)*100.0 + 0.5);

/*
// Debugging - shadow map "raw"
	// 640x480
	float s = gl_FragCoord.x / 640.0;
	s = s * 2.0 - 1.0;
	float t = gl_FragCoord.y / 480.0;
	t = t * 2.0 - 1.0;
	vec2 st = vec2(s, t);
	float shadowMap = texture(textureUnit, st).x;
	out_Color =	 vec4(shadowMap);
*/
}
