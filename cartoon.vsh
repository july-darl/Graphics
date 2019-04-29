#version 450 core

uniform mat4 ModelMatrix;
uniform mat4 IT_ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectMatrix;

uniform vec3 cameraPos;
uniform vec3 LightLocation;

attribute vec4 a_position;
attribute vec3 a_normal;
attribute vec2 a_texcoord;

varying vec2 v_texcoord;
varying vec3 v_normal;
varying vec3 ViewDir;
varying vec3 lightDir;
varying float v_depth;

void main(void)
{
    gl_Position = ModelMatrix * a_position;
    vec3 worldPos = vec3(gl_Position);
    gl_Position = ViewMatrix * gl_Position;
    gl_Position = ProjectMatrix * gl_Position;

    ViewDir = normalize( (cameraPos - worldPos) );
    lightDir = normalize( (LightLocation - worldPos));
    v_texcoord = a_texcoord;
    v_normal = normalize(mat3(IT_ModelMatrix) * a_normal);
    v_depth = worldPos.z;
}
