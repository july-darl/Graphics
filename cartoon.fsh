#version 450 core

uniform float zFar;
varying vec2 v_texcoord;
varying vec3 v_normal;
varying float v_depth;
varying vec3 ViewDir;
varying vec3 lightDir;

layout(location = 0) out vec4 Color;
layout(location = 1) out vec4 Normal;

void main(void)
{
    vec3 reflectDir = normalize(reflect(-lightDir, v_normal));
    float diffuse = 0.6 * clamp(dot(v_normal, lightDir), 0, 1);
    float specular = 0.7 * pow(clamp(dot(reflectDir,ViewDir),0,1),12);
    float ambient = 0.2;
    float color =  clamp(ambient + diffuse + specular,0,1);
    if(color <= 0.3)
        color = 0.4;
    else if(color <= 0.6)
        color = 0.5;

    else if(color <= 0.9)
        color = 0.7;

    else if(color <= 1)
        color = 0.9;

    Normal.xyz = v_normal;
    Normal.w = abs(v_depth / zFar);

    Color.r = Color.b = color;
    Color.g = color/2;

    Color.w = 1;
}
