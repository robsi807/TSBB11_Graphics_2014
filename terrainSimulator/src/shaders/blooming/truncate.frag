#version 150

in vec2 outTexCoord;
uniform sampler2D texUnit;
out vec4 out_Color;

void main(void)
{
	
    vec4 col = texture(texUnit, outTexCoord);
    out_Color.r = max(0,col.r-1);
    out_Color.g = max(0,col.g-1);
    out_Color.b = max(0,col.b-1);
     
}
