#version 150

// terrain.vert
// A phong shader with procedural terrain texturing
// Author: Peter Thulin

in  vec3 inPosition;
in  vec3 inNormal;
in vec2 inTexCoord;

out vec2 texCoord;
out vec3 exNormal;
out vec3 exPosition;
out vec3 surf;
out float cameraDist;

// NY
uniform mat4 projMatrix;
uniform mat4 mdl2World;
uniform mat4 world2View;

void main(void)
{
	mat3 normalMatrix = mat3(world2View * mdl2World);
	exNormal = normalMatrix*inNormal;
	texCoord = inTexCoord;
	vec4 worldPosition =  mdl2World * vec4(inPosition, 1.0);
	gl_Position = projMatrix * world2View * worldPosition;
	surf = normalize(vec3(world2View * mdl2World * vec4(inPosition, 1.0)));
	exPosition = vec3(world2View * mdl2World * vec4(inPosition, 1.0));

	// TODO: Better to upload as uniform once ??
	mat4 viewModel = inverse(world2View);
  	vec4 homogCameraPos = viewModel[3];
	vec3 cameraPos = homogCameraPos.xyz/homogCameraPos.w;
	cameraDist = length(cameraPos - vec3(worldPosition));
}
