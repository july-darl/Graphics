#version 450 core

uniform float zFar;
uniform float rough;
uniform float metal;
uniform float ao;
uniform float alpha;
uniform vec3 color;
uniform sampler2D albedo;
uniform sampler2D normal;
uniform sampler2D mask;
uniform bool bUseNormalMap;
uniform int id;

in vec2 v_texcoord;
in vec3 v_tangent;
in vec3 v_normal;
in vec2 v_depth;
in float v_fireRatio;

layout(location = 0) out vec4 NormalAndDepth;
layout(location = 1) out vec4 Color;
layout(location = 2) out vec4 Param;

vec3 UnpackNormal(vec3 n)
{
    vec3 N = normalize(v_normal);
    vec3 T = normalize(v_tangent - N * v_tangent * N);
    vec3 B = cross(N, T);
    mat3 TBN = mat3(T,B,N);
    n = normalize(2 * n - 1);
    n = normalize(TBN * n);
    return n;
}

void main(void)
{
    if(bUseNormalMap)
    {
        vec3 N = texture(normal, v_texcoord).xyz;
        NormalAndDepth.xyz = (UnpackNormal(N) + 1)/2;
    }
    else
    {
        NormalAndDepth.xyz = (normalize(v_normal) + 1)/2;
    }
    NormalAndDepth.w = -v_depth.x/v_depth.y/zFar;

    Color.xyz = color;
    if(Color.xyz == vec3(-1,-1,-1))
    {
        Color = texture(albedo,v_texcoord);
        if(Color.w < alpha) discard;
    }


    Param.x = rough;
    Param.z = ao;

    if(Param.x == -1)
        Param.x = texture(mask,v_texcoord).x;

    if(Param.z == -1)
        Param.z = texture(mask,v_texcoord).y;

    Param.y = metal;
    Param.w = float(id) / 100;
}
