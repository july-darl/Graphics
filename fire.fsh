#version 450 core

uniform bool bBloom;
uniform vec3 cameraPos;
uniform float zFar;
uniform float zNear;
uniform sampler2D fire;
uniform vec3 windDir;

in vec2 v_texcoord;
in vec3 v_normal;
in vec3 v_tangent;
in vec3 v_worldPos;
in vec2 v_depth;

out vec4 fragColor;

vec3 World2Tangent(vec3 dir)
{
    vec3 N = normalize(v_normal);
    vec3 T = normalize(v_tangent - N * v_tangent * N);
    vec3 B = cross(N, T);
    mat3 TBN = mat3(T,B,N);

    vec3 ret = transpose(TBN) * dir;

    return normalize(ret);
}

const float flame_engulf = 12.09;
void main(void)
{
    float NdotD = dot(normalize(windDir),normalize(v_normal));

    vec3 fire_base = vec3(0.585,0,0); // red
    vec3 fire_mod = vec3(0.975,0.553,0); // yellow

    float base = 0.5 * (NdotD + 1); // [0,1]
    float p = pow(base, flame_engulf); // define energe

    float depth = (-v_depth.x/v_depth.y - zNear)/zFar * 20;

    vec3 viewDir = normalize(cameraPos - v_worldPos);
    vec3 tangent_viewDir = World2Tangent(viewDir);

    float z = tangent_viewDir.z;

    float t = 1 - pow(z,5);
    t = clamp(t, 0, 1);
    float alpha =  t * clamp(NdotD,0,1);

    vec3 fire_color = mix(fire_mod,fire_base,alpha) * p * clamp(depth,0,1);

    fragColor = vec4(fire_color,0.5);
}
