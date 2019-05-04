#version 450 core

in vec2 v_depth;
varying vec4 fragColor;
void main()
{
    float fColor =  (v_depth.x/v_depth.y + 1)/2;
    fragColor.x = fColor;
}
