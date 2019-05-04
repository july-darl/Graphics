
#version 450 core

uniform mat4 OrthoMatrix;
uniform mat4 SnowMatrix;
uniform mat4 ModelMatrix;

in vec4 a_position;

out vec2 v_depth;

void main()
{
    gl_Position = ModelMatrix * a_position;
    gl_Position = SnowMatrix * gl_Position;
    gl_Position = OrthoMatrix * gl_Position;

    v_depth = gl_Position.zw;
}
