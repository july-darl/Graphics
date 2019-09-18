#version 450 core

uniform mat4 ModelMatrix;
uniform mat4 IT_ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectMatrix;

uniform vec3 color;
uniform sampler2D albedo;

in vec2 v_texcoord;
in vec3 v_normal;
in vec3 v_tangent;

void main(void)
{
    if(color.rgb == vec3(-1,-1,-1))
    {
        gl_FragColor.rgb = texture(albedo,v_texcoord).rgb;
    }
    else
    {
        gl_FragColor.rgb = color;
    }

    gl_FragColor.a = 0.5;
}
