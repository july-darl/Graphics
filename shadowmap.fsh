#version 450 core

varying vec2 v_depth;
uniform float zFar;

void main()
{
    float fColor =  -(v_depth.x/v_depth.y)/zFar;
    gl_FragColor.x = fColor;
}
