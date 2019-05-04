#version 450 core

uniform float zFar;
in vec2 v_depth;

out vec4 fragColor;
void main()
{
    float fColor =  -(v_depth.x/v_depth.y)/zFar;
    fragColor.x = fColor;
}
