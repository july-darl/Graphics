#version 450 core
uniform float fov;
uniform float zFar;
uniform float aspect;
attribute vec4 a_position;
attribute vec2 a_texcoord;

varying vec2 v_texcoord;
varying vec2 farPlanePos;

void main()
{
    gl_Position = a_position;
    v_texcoord = a_texcoord;
    float t = tan(fov/2);
    farPlanePos.x = (v_texcoord.x * 2 - 1) * zFar * t * aspect;
    farPlanePos.y = (v_texcoord.y * 2 - 1) * zFar * t;
}
