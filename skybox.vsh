#version 450 core

uniform mat4 ProjectMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ModelMatrix;

in vec4 a_position;
in vec2 a_texcoord;

out vec3 localPos;

void main()
{
    mat4 rotView = mat4(mat3(ViewMatrix));
    localPos = a_position.xyz;
    gl_Position =  ProjectMatrix * (rotView * (ModelMatrix * a_position));

    gl_Position = gl_Position.xyww;
}
