#version 450 core

uniform samplerCube envCubemap;
in vec3 localPos;

layout(location = 0) out vec4 NormalAndDepth;
layout(location = 1) out vec4 Color;
layout(location = 2) out vec4 Param;

void main()
{
    vec3 pos = normalize(localPos);
    vec3 envColor = texture(envCubemap, pos).rgb;

    envColor = envColor / (envColor + vec3(1.0));
    envColor = pow(envColor, vec3(1.0/2.2));

    Color = vec4(envColor,1);

    if(abs(localPos.y - 1) < 0.01)
    {
        pos = (pos + 1)/2;
        NormalAndDepth = vec4(1,1,pos.x,pos.z);
    }
    else
    {
        NormalAndDepth = vec4(1,1,1,1);
    }
}
