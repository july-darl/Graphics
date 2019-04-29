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
attribute vec3 a_tangent;
attribute vec2 a_texcoord;

varying vec2 v_texcoord;
varying vec3 v_tangent;
varying vec3 v_normal;
varying vec2 v_depth;

void main()
{
    gl_Position = ModelMatrix * a_position;
    gl_Position = ViewMatrix  * gl_Position;

    v_depth = gl_Position.zw;
    gl_Position = ProjectMatrix * gl_Position;

    mat3 M1 = mat3(IT_ModelMatrix[0].xyz, IT_ModelMatrix[1].xyz, IT_ModelMatrix[2].xyz);
    v_normal = normalize(M1 * a_normal);

    mat3 M2 = mat3(ModelMatrix[0].xyz, ModelMatrix[1].xyz, ModelMatrix[2].xyz);
    v_tangent = normalize(M2 * a_tangent);

    v_texcoord = a_texcoord;
}
