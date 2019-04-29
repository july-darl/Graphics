#version 450 core

uniform sampler2D p1;
uniform sampler2D p2;
uniform sampler2D p3;
uniform sampler2D p4;
uniform sampler2D p5;
uniform sampler2D p6;

layout(location = 0) out vec4 Color;
layout(location = 1) out vec4 Normal;

varying vec3 v_normal;
varying vec2 v_texcoord;
varying float param1,param2,param3,param4,param5,param6,param7;

void main()
{
    Normal.xyz = v_normal;
    vec2 uv = v_texcoord;
    Color =
            param1 * texture2D(p6,uv) +
            param2 * texture2D(p5,uv) +
            param3 * texture2D(p4,uv) +
            param4 * texture2D(p3,uv) +
            param5 * texture2D(p2,uv) +
            param6 * texture2D(p1,uv) +
            param7 * vec4(1,1,1,1);
}
