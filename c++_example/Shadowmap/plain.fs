#version 150

uniform float shade;
out vec4 out_Color;

void main(void)
{
	out_Color = vec4(shade);
}
