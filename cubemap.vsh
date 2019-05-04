#version 450 core

uniform mat4 ProjectMatrix;
uniform mat4 ViewMatrix;

in vec4 a_position;
in vec3 a_normal;
in vec2 a_texcoord;

out vec3 localPos;

void main()
{
    localPos = vec3(a_position);
    gl_Position = ProjectMatrix * (ViewMatrix * a_position);
}
