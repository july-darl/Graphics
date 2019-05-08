#version 450 core
uniform sampler2D Mask;
uniform sampler2D albedo;
uniform vec3 color;
uniform bool bBloom;
uniform vec3 cameraPos;

in vec2 v_texcoord;
out vec4 fragColor;

const float flame_engulf = 12.09;
void main(void)
{
    bool bMask = texture(Mask, v_texcoord).r > 0.8;

    if(bBloom)
    {
        vec3 bright_color;
        if(color == vec3(-1,-1,-1))
        {
            bright_color = texture(albedo,v_texcoord).xyz;
        }
        else
        {
            bright_color = color;
        }
        if(bright_color.r > 0.95 || bright_color.b > 0.8 || bright_color.g > 0.8)
        {
            fragColor = vec4(bright_color,1);
        }
        else
        {
             fragColor = vec4(0,0,0,1);
        }
    }
    else
    {
        fragColor = vec4(0,0,0,1);
    }
}
