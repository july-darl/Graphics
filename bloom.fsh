#version 450 core
uniform sampler2D Mask;
uniform sampler2D albedo;
uniform vec3 color;
uniform int width;
uniform int height;
uniform bool bBloom;
uniform bool bFire;
uniform sampler2D fire;

in vec2 v_texcoord;
in float v_fireRatio;

out vec4 fragColor;

void main(void)
{
    bool bMask = texture(Mask, v_texcoord).r > 0.8;
    vec3 bright_color;
    if(v_fireRatio > 0)
    {
        vec3 fire_color = texture(fire,v_texcoord).xyz;
        if(color == vec3(-1,-1,-1))
        {
            bright_color = texture(albedo,v_texcoord).xyz;
        }
        else
        {
            bright_color = color;
        }
        fragColor = vec4(mix(bright_color,fire_color,v_fireRatio),1);
    }

    else if(bBloom)
    {
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
