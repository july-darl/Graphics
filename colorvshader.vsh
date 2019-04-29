#version 450 core
#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

uniform mat4 ModelMatrix;
uniform mat4 IT_ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectMatrix;
attribute vec4 a_position;
attribute vec3 a_normal;
attribute vec2 a_texcoord;

varying vec2 v_depth;
varying vec3 v_normal;
void main()
{
    gl_Position = ModelMatrix * a_position;
    gl_Position = ViewMatrix * gl_Position;
    v_depth = gl_Position.zw;
    gl_Position = ProjectMatrix * gl_Position;

    mat3 M = mat3(IT_ModelMatrix[0].xyz, IT_ModelMatrix[1].xyz, IT_ModelMatrix[2].xyz);
    v_normal = M * a_normal;
}
