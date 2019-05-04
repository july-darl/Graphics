#version 450

uniform mat4 ProjectMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ModelMatrix;
uniform mat4 IT_ModelMatrix;

in vec4 a_position;
in vec2 a_texcoord;
in vec3 a_normal;

out vec3 v_worldPos;

void main(void)
{
    gl_Position = ModelMatrix * a_position;
    v_worldPos = gl_Position.xyz / gl_Position.w;

    mat4 rotView = mat4(mat3(ViewMatrix));

    gl_Position = ViewMatrix * gl_Position;

    gl_Position = ProjectMatrix * gl_Position;
}
