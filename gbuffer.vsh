#version 450 core

uniform mat4 ModelMatrix;
uniform mat4 IT_ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectMatrix;
uniform bool bFire;
uniform sampler2D perlin;
uniform float time;

in vec4 a_position;
in vec3 a_normal;
in vec3 a_tangent;
in vec2 a_texcoord;

out vec2 v_texcoord;
out vec3 v_tangent;
out vec3 v_normal;
out vec2 v_depth;

void main()
{

    mat3 M1 = mat3(IT_ModelMatrix[0].xyz, IT_ModelMatrix[1].xyz, IT_ModelMatrix[2].xyz);
    v_normal = normalize(M1 * a_normal);

    mat3 M2 = mat3(ModelMatrix[0].xyz, ModelMatrix[1].xyz, ModelMatrix[2].xyz);
    v_tangent = normalize(M2 * a_tangent);


    v_texcoord = a_texcoord;

    gl_Position = ModelMatrix * a_position;
    gl_Position = ViewMatrix  * gl_Position;

    v_depth = gl_Position.zw;
    gl_Position = ProjectMatrix * gl_Position;
}
