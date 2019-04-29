#version 450 core

uniform mat4 ProjectMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ModelMatrix;

attribute vec4 a_position;
attribute vec3 a_normal;
attribute vec2 a_texcoord;

varying vec3 localPos;

void main()
{
    mat4 rotView = mat4(mat3(ViewMatrix));
    localPos = a_position.xyz;
    gl_Position =  ProjectMatrix * rotView * (ModelMatrix * a_position);
    gl_Position = gl_Position.xyww;
}
