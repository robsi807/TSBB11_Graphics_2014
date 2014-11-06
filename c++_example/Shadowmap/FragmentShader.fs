#version 150

// Projected texture demo
// Based on a demo by Fabien Sanglard, http://www.fabiensanglard.net/shadowmapping/index.php

uniform sampler2D textureUnit;
in vec4 lightSourceCoord;
in vec3 exNormal;
in vec3 exSurface;
out vec4 out_Color;
uniform float shade;

#define EPSILON 0.00001
/*
float CalcShadowFactor(vec4 LightSpacePos)
{
    vec3 ProjCoords = LightSpacePos.xyz / LightSpacePos.w;
    vec2 UVCoords;
    UVCoords.x = 0.5 * ProjCoords.x + 0.5;
    UVCoords.y = 0.5 * ProjCoords.y + 0.5;
    float z = 0.5 * ProjCoords.z + 0.5;

    float xOffset = 1.0/1024;
    float yOffset = 1.0/768;

    float Factor = 0.0;

    for (int y = -1 ; y <= 1 ; y++) {
        for (int x = -1 ; x <= 1 ; x++) {
            vec2 Offsets = vec2(x * xOffset, y * yOffset);
            vec3 UVC = vec3(UVCoords + Offsets, z + EPSILON);
            Factor += texture(textureUnit, UVC);
        }
    }

    return (0.5 + (Factor / 18.0));
}
*/
void main()
{	
	
	// Perform perspective division to get the actual texture position
	vec4 shadowCoordinateWdivide = lightSourceCoord / lightSourceCoord.w;
	
	// Used to lower moire' pattern and self-shadowing
	// The optimal value here will vary with different GPU's depending on their Z buffer resolution.
	shadowCoordinateWdivide.z -= 0.002;
	
	// Look up the depth value
	float distanceFromLight = texture(textureUnit, shadowCoordinateWdivide.st).x;
	distanceFromLight = (distanceFromLight-0.5) * 2.0;
	
	// NOTE: This distance look-up disturbs me. It is too simple. It should really
	// use the camera parameters to correctly restore the actual distance.
	// For the moment I don't have time to fix this. The demo works, but it
	// may have hit some constants that are correct more by luck than skill.
	// This is regrettable and I will correct this when I have time. In the meantime
	// I do not want to withhold the demo. /Ingemar
	
	// Compare
	float shadow = 1.0; // 1.0 = no shadow
	float bias = 0.005 ; //tan(acos(clamp(dot(exNormal,exSurface-vec3(lightSourceCoord)),0,1)));
	if (lightSourceCoord.w > 0.0)
		if (distanceFromLight + bias < shadowCoordinateWdivide.z) // shadow
			shadow = 0.5;
	
	//float shadow = CalcShadowFactor(lightSourceCoord);
	
	out_Color = vec4(shadow * shade);



// Debugging - other data mapped on scene
//	out_Color =	 vec4(distanceFromLight); // Dvs Z-buffer
//	out_Color =	 vec4(shadowCoordinateWdivide.z); // Bara avstŒnd

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
