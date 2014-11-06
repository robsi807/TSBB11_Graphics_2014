#version 150

// Projected texture demo
// Based on a demo by Fabien Saglard, http://www.fabiensanglard.net/shadowmapping/index.php

out vec4 lightSourceCoord;
uniform int texunit;

in vec3 in_Position;
in vec3 inNormal;

out vec3 exNormal;
out vec3 exSurface;

uniform mat4 textureMatrix;
uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;

void main()
{
	lightSourceCoord = textureMatrix * vec4(in_Position, 1.0); // Transform vertex to light source coordinates

	gl_Position = projectionMatrix*modelViewMatrix * vec4(in_Position, 1.0);
	exSurface = vec3(modelViewMatrix * vec4(in_Position, 1.0));
	exNormal = inverse(transpose(mat3(modelViewMatrix))) * inNormal;
}
