#version 150

in vec2 outTexCoord;
uniform sampler2D texUnit;
out vec4 out_Color;

void main(void)
{
	const float width = 1024.0;
	const float height = 860.0;
	
vec4 blur = (4*texture(texUnit,outTexCoord) +
	2*texture(texUnit,outTexCoord+vec2(1/width,0))  +
	2*texture(texUnit,outTexCoord+vec2(-1/width,0)) +
	2*texture(texUnit,outTexCoord+vec2(0,1/height)) +
	texture(texUnit,outTexCoord+vec2(1/width,1/height)) +
	texture(texUnit,outTexCoord+vec2(-1/width,1/height)) +
	2*texture(texUnit,outTexCoord+vec2(0,-1/height)) +
	texture(texUnit,outTexCoord+vec2(1/width,-1/height)) +
	texture(texUnit,outTexCoord+vec2(-1/width,-1/height)))/16;
	
	out_Color = blur;
}
