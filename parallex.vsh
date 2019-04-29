#version 450 core

uniform mat4 ModelMatrix;
uniform mat4 IT_ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectMatrix;
uniform vec3 cameraPos;
uniform vec3 LightLocation;
uniform mat4 LightMatrix;

attribute vec4 a_position;
attribute vec3 a_normal;
attribute vec3 a_tangent;
attribute vec2 a_texcoord;

varying vec2 v_texcoord;
varying vec3 v_normal;
varying vec3 ViewDir;
varying vec3 lightDir;
varying vec4 lightPos;

void main()
{
    gl_Position = ModelMatrix * a_position;
    vec3 worldPos = vec3(gl_Position);
    gl_Position = ViewMatrix * gl_Position;
    gl_Position = ProjectMatrix * gl_Position;
    v_texcoord = a_texcoord;

    v_normal = mat3(IT_ModelMatrix) * a_normal;
    vec3 tangent = mat3(ModelMatrix) * a_tangent;

    ViewDir = normalize( (cameraPos - worldPos) );
    lightDir = normalize( (LightLocation - worldPos));
    vec3 N = normalize(v_normal);
    vec3 T = normalize(tangent - N * tangent * N);
    vec3 B = cross(N, T);
    mat3 TBN = mat3(T,B,N);


    lightPos = ProjectMatrix * (LightMatrix * (ModelMatrix * a_position));

    ViewDir = normalize( (cameraPos - worldPos) * TBN);
    lightDir = normalize( (LightLocation - worldPos) * TBN);
}

