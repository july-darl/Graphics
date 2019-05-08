#version 450 core

in vec4 a_position;
in vec3 a_normal;
in vec3 a_tangent;
in vec2 a_texcoord;

out vec2 v_texcoord;
out vec3 v_normal;
out vec3 v_tangent;

void main()
{
    gl_Position = a_position;

    v_normal = a_normal;
    v_tangent = a_tangent;
    v_texcoord = a_texcoord;
}
