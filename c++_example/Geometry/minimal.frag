#version 150
out vec4 outColor;in vec3 exNormalG; in vec2 texCoordG;uniform sampler2D tex;void main(void){	const vec3 light = vec3(0.58, 0.58, 0.58);	float shade;		shade = dot(normalize(exNormalG), light);	outColor = vec4(shade, shade, shade, 1.0);}
