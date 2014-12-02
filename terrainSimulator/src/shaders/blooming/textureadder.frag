#version 150

in vec2 outTexCoord;
uniform sampler2D texUnit1;
uniform sampler2D texUnit2;
out vec4 out_Color;

void main(void)
{
    out_Color = texture(texUnit1,outTexCoord) + 0.3*texture(texUnit2, outTexCoord);
}
