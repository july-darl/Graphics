#version 450 core

in vec4 a_position;
in vec3 a_normal;
in vec3 a_tangent;
in vec2 a_texcoord;

uniform mat4 ModelMatrix;
uniform mat4 IT_ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectMatrix;
uniform bool bFire;
uniform sampler2D perlin;

out vec2 v_texcoord;
out float v_fireRatio;

void main()
{
    mat3 M1 = mat3(IT_ModelMatrix[0].xyz, IT_ModelMatrix[1].xyz, IT_ModelMatrix[2].xyz);
    vec3 normal = normalize(M1 * a_normal);

    v_fireRatio = 0;
    if(bFire)
    {
        vec3 dir = normalize(vec3(0.3,1,0));
        if(dot(normal,dir) > 0.7)
        {
            v_fireRatio = 1;
        }

    }

    if(bFire)
    {
        gl_Position = a_position;
    }
    else
    {
        gl_Position = ProjectMatrix * (ViewMatrix * (ModelMatrix * a_position));
    }
    v_texcoord = a_texcoord;

}
