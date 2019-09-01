#version 450 core

layout(location = 0) out vec4 Normal;
layout(location = 1) out vec4 Color;

void main(void)
{
    Color = vec4(1,0,0,1);
    Normal = vec4(1,1,1,1);
}
