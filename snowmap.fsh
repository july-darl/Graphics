#version 450 core

varying vec2 v_depth;

void main()
{
    float fColor =  (v_depth.x/v_depth.y + 1)/2;
    gl_FragColor.x = fColor;
}
