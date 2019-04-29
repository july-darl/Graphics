#version 450

uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectMatrix;
uniform bool bFixZ;
attribute vec4 a_position;
attribute vec2 a_texcoord;
varying vec2 v_texcoord;
void main(void)
{
    gl_Position = ModelMatrix * a_position;
    gl_Position = ViewMatrix * gl_Position;
    if(bFixZ)
    {
        gl_Position.z = gl_Position.w * -30;
    }
    gl_Position = ProjectMatrix * gl_Position;

    gl_Position.z = gl_Position.w * -0.5;
    v_texcoord = a_texcoord;
}
