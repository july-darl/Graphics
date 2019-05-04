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
uniform mat4 LightMatrix;
uniform mat4 OrthoMatrix;
in vec4 a_position;
in vec3 a_normal;
in vec2 a_texcoord;

out vec2 v_texcoord;
out vec3 v_normal;
out vec3 worldPos;
out vec4 lightPos;

void main()
{
    gl_Position = ModelMatrix * a_position;
    worldPos = vec3(gl_Position);

    gl_Position = ViewMatrix * gl_Position;

    gl_Position = ProjectMatrix * gl_Position;

    v_texcoord = a_texcoord;

    mat3 M = mat3(IT_ModelMatrix[0].xyz, IT_ModelMatrix[1].xyz, IT_ModelMatrix[2].xyz);
    v_normal = M * a_normal;

    lightPos = OrthoMatrix * (LightMatrix * (ModelMatrix * a_position));
}
