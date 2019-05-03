#version 450 core

uniform mat4 ModelMatrix;
uniform mat4 IT_ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectMatrix;
uniform float time;
uniform bool bFire;
uniform sampler2D perlin;

attribute vec4 a_position;
attribute vec3 a_normal;
attribute vec3 a_tangent;
attribute vec2 a_texcoord;

varying float v_fireRatio;
varying vec2 v_texcoord;
varying vec3 v_tangent;
varying vec3 v_normal;
varying vec2 v_depth;

//layout (vertices = 4) out;

void main()
{
    mat3 M1 = mat3(IT_ModelMatrix[0].xyz, IT_ModelMatrix[1].xyz, IT_ModelMatrix[2].xyz);
    v_normal = normalize(M1 * a_normal);

    mat3 M2 = mat3(ModelMatrix[0].xyz, ModelMatrix[1].xyz, ModelMatrix[2].xyz);
    v_tangent = normalize(M2 * a_tangent);

    vec3 pos = a_position.xyz;
    v_fireRatio = 0;
    if(bFire)
    {
        vec3 dir = normalize(vec3(0.3,1,0));
        if(dot(v_normal,dir) > 0.7)
        {
            v_fireRatio = 1;
            float offset = texture2D(perlin,vec2(pos.x,pos.z) /10 + time).r;
            offset = clamp((offset - 0.5) / (1 - 0.5),0,1);
            pos += dir * 0.5 * offset;
        }

    }

    gl_Position = vec4(pos,1);
    gl_Position = ModelMatrix * vec4(pos,1);
    gl_Position = ViewMatrix  * gl_Position;

    v_depth = gl_Position.zw;
    gl_Position = ProjectMatrix * gl_Position;

    v_texcoord = a_texcoord;
}
