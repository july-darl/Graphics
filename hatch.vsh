#version 450 core

uniform mat4 ModelMatrix;
uniform mat4 IT_ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectMatrix;
uniform vec3 cameraPos;
uniform vec3 LightLocation;

attribute vec4 a_position;
attribute vec3 a_normal;
attribute vec3 a_tangent;
attribute vec2 a_texcoord;

varying vec3 v_normal;
varying vec2 v_texcoord;

varying float param1,param2,param3,param4,param5,param6,param7;
void main()
{
    gl_Position = ModelMatrix * a_position;
    vec3 worldPos = vec3(gl_Position);
    gl_Position = ViewMatrix * gl_Position;
    gl_Position = ProjectMatrix * gl_Position;
    v_texcoord = a_texcoord;

    v_normal = mat3(IT_ModelMatrix) * a_normal;
    vec3 ViewDir = normalize(cameraPos - worldPos);
    vec3 lightDir = normalize(LightLocation - worldPos);
    vec3 reflectDir = normalize(reflect(-lightDir,v_normal));
    float diffuse = 0.9 * clamp(dot(v_normal, lightDir), 0, 1);
    float specular = 0.3 * pow(clamp(dot(reflectDir,ViewDir),0,1),15);
    float ambient = 0.1;

    float ratio = clamp(diffuse + specular + ambient, 0.0f, 1.0f) * 6;

    param1 = 0.0f;
    param2 = 0.0f;
    param3 = 0.0f;
    param4 = 0.0f;
    param5 = 0.0f;
    param6 = 0.0f;
    param7 = 0.0f;
    if(ratio <= 1)
    {
         param2 = ratio;
         param1 = 1 - ratio;
    }
    else if(ratio <= 2)
    {
        ratio -= 1;
        param3 = ratio;
        param2 = 1 - ratio;
    }
    else if(ratio <= 3)
    {
        ratio -= 2;
        param4 = ratio;
        param3 = 1 - ratio;
    }
    else if(ratio <= 4)
    {
        ratio -= 3;
        param5 = ratio;
        param4 = 1 - ratio;
    }
    else if(ratio <= 5)
    {
        ratio -= 4;
        param6 = ratio;
        param5 = 1 - ratio;
    }
    else if(ratio <= 6)
    {
        ratio -= 5;
        param7 = ratio;
        param6 = 1 - ratio;
    }
}

