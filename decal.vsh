#version 450

uniform mat4 ViewMatrix;
uniform mat4 ProjectMatrix;
uniform mat4 decalToWorld;
uniform float decalSize;


in vec4 a_position;
in vec2 a_texcoord;

out vec2 v_texcoord;
out vec4 v_projPos;
out vec2 farPlanePos;

void main(void)
{
    // scale the unit cube and position it in world space
    vec4 worldPos = decalToWorld * vec4(a_position.xyz * decalSize, a_position.w);
    gl_Position = ViewMatrix * worldPos;
    gl_Position = ProjectMatrix * gl_Position;

    v_projPos = gl_Position;
    v_texcoord = a_texcoord;

}
