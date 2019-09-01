#version 450 core

uniform mat4 ProjectMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ModelMatrix;

in vec4 a_position;
in vec3 a_normal;
in vec2 a_texcoord;

void main()
{
    gl_Position = ProjectMatrix * (ViewMatrix * ModelMatrix * a_position);
}
