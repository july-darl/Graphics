#version 450 core

in vec2 v_texcoord[gl_MaxPatchVertices];
in vec3 v_normal[gl_MaxPatchVertices];
in vec3 v_tangent[gl_MaxPatchVertices];

out vec2 c_texcoord[gl_MaxPatchVertices];
out vec3 c_normal[gl_MaxPatchVertices];
out vec3 c_tangent[gl_MaxPatchVertices];

layout (vertices = 3) out;

void main(void)
{
    gl_TessLevelInner[0] = 10.0;
    gl_TessLevelOuter[0] = 10.0;
    gl_TessLevelOuter[1] = 10.0;
    gl_TessLevelOuter[2] = 10.0;

    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

    c_texcoord[gl_InvocationID] = v_texcoord[gl_InvocationID];
    c_normal[gl_InvocationID] = v_normal[gl_InvocationID];
    c_tangent[gl_InvocationID] = v_tangent[gl_InvocationID];
}
