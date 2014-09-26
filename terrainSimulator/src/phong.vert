#version 150

in  vec3 inPosition;
in  vec3 inNormal;
in vec2 inTexCoord;

out vec2 texCoord;
out vec3 exNormal;
out vec3 exPosition;
out vec3 surf;

// NY
uniform mat4 projMatrix;
uniform mat4 mdl2World;
uniform mat4 world2View;

void main(void)
{
	mat3 normalMatrix1 = mat3(world2View * mdl2World);
	exNormal = normalMatrix1*inNormal;
	texCoord = inTexCoord;
	gl_Position = projMatrix * world2View * mdl2World * vec4(inPosition, 1.0);
	surf = normalize(vec3(world2View * mdl2World * vec4(inPosition, 1.0)));
	exPosition = vec3(world2View * mdl2World * vec4(inPosition, 1.0));
}
