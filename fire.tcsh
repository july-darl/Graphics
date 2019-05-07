#version 450 core

uniform vec3 windDir;
uniform mat4 IT_ModelMatrix;

in vec2 v_texcoord[gl_MaxPatchVertices];
in vec3 v_normal[gl_MaxPatchVertices];
in vec3 v_tangent[gl_MaxPatchVertices];

out vec2 c_texcoord[gl_MaxPatchVertices];
out vec3 c_normal[gl_MaxPatchVertices];
out vec3 c_tangent[gl_MaxPatchVertices];

layout (vertices = 3) out;

const float flame_engulf = 4.09;

float ComputeSize()
{
    vec3 normal = v_normal[gl_InvocationID];
    mat3 M1 = mat3(IT_ModelMatrix[0].xyz, IT_ModelMatrix[1].xyz, IT_ModelMatrix[2].xyz);
    normal = normalize(M1 * normal);


    float NdotD = dot(normal,windDir);
    float r = 0.5 * (NdotD + 1); // to [0,1]


    if(r > 0.8)
    {
        return 8;
    }
    if(r > 0.5)
    {
        return 6;
    }
    if(r > 0.3)
    {
        return 4;
    }
    return 2;
}

void main(void)
{

    //float size = ComputeSize();
    gl_TessLevelInner[0] = 8;
    gl_TessLevelOuter[0] = 8;
    gl_TessLevelOuter[1] = 8;
    gl_TessLevelOuter[2] = 8;

    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

    c_texcoord[gl_InvocationID] = v_texcoord[gl_InvocationID];
    c_normal[gl_InvocationID] = v_normal[gl_InvocationID];
    c_tangent[gl_InvocationID] = v_tangent[gl_InvocationID];
}
