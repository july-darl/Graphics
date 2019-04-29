#version 450 core

uniform mat4 ProjectMatrix;
uniform mat4 ViewMatrix;

attribute vec4 a_position;
attribute vec3 a_normal;
attribute vec2 a_texcoord;

varying vec3 localPos;
void main()
{
    localPos = vec3(a_position);
    gl_Position = ProjectMatrix * (ViewMatrix * a_position);
}
