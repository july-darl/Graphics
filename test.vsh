#version 450 core

in vec4 a_position;
void main(void)
{
    gl_Position = a_position/3;//vertices[gl_VertexID];
}
