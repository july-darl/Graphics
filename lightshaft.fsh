#version 450 core

in vec2 v_texcoord;

uniform vec3 cameraPos;
uniform sampler2D NormalAndDepth;
uniform sampler2D ShadowMap;
uniform mat4 Inverse_ViewMatrix;
uniform mat4 LightMatrix;
uniform mat4 ProjectMatrix;

uniform float g;
uniform float zFar;
uniform float zNear;
uniform vec3 lightPosition;
uniform int ScreenX;
uniform int ScreenY;

varying vec2 farPlanePos;

layout(location = 0) out vec4 LightTex;

vec3 yellow_light = vec3(1,198.0/255.0,107.0/255.0);

mat4 dither = mat4(
   0,       0.5,    0.125,  0.625,
   0.75,    0.25,   0.875,  0.375,
   0.1875,  0.6875, 0.0625, 0.5625,
   0.9375,  0.4375, 0.8125, 0.3125
);

vec3 ComputeWorldPos(float depth)
{
    vec4 pos = vec4(vec3(farPlanePos.x, farPlanePos.y,-zFar) * depth , 1);
    vec4 ret = Inverse_ViewMatrix * pos;
    return ret.xyz / ret.w;
}

bool IsInShadow(vec4 worldPos)
{
    float fShadow = 0.0;
    vec4 lightPos = (LightMatrix * (worldPos));
    float fDistance = -lightPos.z / zFar;
    lightPos = ProjectMatrix * lightPos;

    vec2 uv = lightPos.xy / lightPos.w * 0.5 + vec2(0.5, 0.5);

    uv.x = clamp(uv.x, 0, 1);
    uv.y = clamp(uv.y, 0, 1);

    float offset = 0.5/zFar;

    float distanceMap = texture2D(ShadowMap, uv).r;
    return fDistance - offset > distanceMap;
}

void main(void)
{
    vec4 result = texture2D(NormalAndDepth, v_texcoord);

    vec3 normal = result.xyz * 2 - 1;
    float depth = result.w;
    vec3 worldPos = ComputeWorldPos(depth);

    float I = 0.0;
    float d = depth * zFar;
    int virtual_plane_num = 100;
    int sampleCoordX = int(mod((ScreenX/2 * v_texcoord.x),4));
    int sampleCoordY = int(mod((ScreenY/2 * v_texcoord.y),4));

    int begin = int(virtual_plane_num * zNear / (d - zNear));
    int end = int(virtual_plane_num * (zFar - d) / (d - zNear));
    int total = begin + virtual_plane_num + end;
    float offset = 1.0 / total * dither[sampleCoordX][sampleCoordY];
    for(int j = begin;j <= virtual_plane_num + begin; j++)
    {
        float z = 1.0 * j / total + offset;

        vec3 pos = ComputeWorldPos(z);
        if(z < depth && !IsInShadow(vec4(pos,1)))
        {
            vec3 lightDis = pos - lightPosition;
            vec3 viewDis = pos - cameraPos;

            float lightDis2 = lightDis.x * lightDis.x + lightDis.y * lightDis.y  + lightDis.z * lightDis.z;

            vec3 lightDir = normalize(lightDis);
            vec3 viewDir = normalize(viewDis);

            float cosTheta = dot(lightDir,normalize(-lightPosition));
            float hg = 1.0/(4*3.14)* (1 - g*g)/ pow(1 + g*g -2*g * dot(lightDir,-viewDir), 1.5);
            if(cosTheta > 0.9)
            {
                I += clamp(10 * hg / virtual_plane_num, 0, 1);
            }
        }
    }
    I = clamp(I ,0, 1);
    vec3 total_light = I * yellow_light;

    LightTex = vec4(total_light,1);
}
