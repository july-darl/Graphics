
#version 450 core

uniform mat4 ProjectMatrix;
uniform mat4 LightMatrix;
uniform mat4 ModelMatrix;

attribute vec4 a_position;

varying vec2 v_depth;

void main()
{
    gl_Position = ModelMatrix * a_position;
    gl_Position = LightMatrix * gl_Position;
    v_depth = gl_Position.zw;
    gl_Position = ProjectMatrix * gl_Position;
}
