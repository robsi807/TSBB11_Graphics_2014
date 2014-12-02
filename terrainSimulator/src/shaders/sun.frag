#version 150

out vec4 outColor;

uniform float alpha;

void main(void)
{
	outColor = 10*vec4(0.9,0.9,0.4,alpha);
}
