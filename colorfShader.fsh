#version 450 core
uniform vec3 color;
uniform float zFar;
out vec3 v_normal;
out vec2 v_depth;
layout(location = 0) out vec4 Color;
layout(location = 1) out vec4 NormalAndDepth;

void main(void)
{
    Color = vec4(color,1);
    NormalAndDepth.xyz = (normalize(v_normal) + 1)/2;
    NormalAndDepth.w = -v_depth.x/v_depth.y/zFar;
}
