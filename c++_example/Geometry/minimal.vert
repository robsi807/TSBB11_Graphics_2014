#version 150
in vec3 inPosition;in vec3 inNormal;in vec2 inTexCoord;out vec3 exNormal; out vec2 texCoord;uniform mat4 modelViewMatrix;uniform mat4 projMatrix;void main(void){	mat3 normalMatrix1 = mat3(modelViewMatrix);	exNormal = normalMatrix1 * inNormal; 		texCoord = inTexCoord;	gl_Position = projMatrix * modelViewMatrix * vec4(inPosition, 1.0);}
