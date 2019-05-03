#version 450 core
uniform sampler2D Mask;
uniform sampler2D albedo;
uniform vec3 color;
uniform int width;
uniform int height;
uniform bool bBloom;
varying vec2 v_texcoord;
varying float v_fireRatio;
uniform sampler2D fire;

void main(void)
{
    bool bMask = texture2D(Mask, v_texcoord).r > 0.8;
    vec3 bright_color;
    if(v_fireRatio > 0)
    {
        vec3 fire_color = texture2D(fire,v_texcoord).xyz;
        if(color == vec3(-1,-1,-1))
        {
            bright_color = texture2D(albedo,v_texcoord).xyz;
        }
        else
        {
            bright_color = color;
        }
        gl_FragColor = vec4(mix(bright_color,fire_color,v_fireRatio),1);
    }

    else if(bBloom)
    {
        if(color == vec3(-1,-1,-1))
        {
            bright_color = texture2D(albedo,v_texcoord).xyz;
        }
        else
        {
            bright_color = color;
        }
        if(bright_color.r > 0.95 || bright_color.b > 0.8 || bright_color.g > 0.8)
        {
            gl_FragColor = vec4(bright_color,1);
        }
        else
        {
             gl_FragColor = vec4(0,0,0,1);
        }
    }
    else
    {
        gl_FragColor = vec4(0,0,0,1);
    }



}
