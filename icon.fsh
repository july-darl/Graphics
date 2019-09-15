#version 330
uniform sampler2D LightTex;
in vec2 v_texcoord;
out vec4 Color;

void main(void)
{
    vec4 color = texture2D(LightTex, v_texcoord);
    if(color.w < 0.9)
        discard;
    Color = color;
}
