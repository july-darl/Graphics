#version 450 core
uniform sampler2D Mask;
uniform sampler2D albedo;
uniform vec3 color;
uniform int width;
uniform int height;
uniform bool bBloom;
uniform bool bFire;
uniform vec3 cameraPos;
uniform float zFar;
uniform sampler2D fire;
uniform vec3 windDir;

in vec2 v_texcoord;
in vec3 v_normal;
in vec3 v_tangent;
in vec2 v_depth;
in vec3 v_worldPos;

out vec4 fragColor;

vec3 N = normalize(v_normal);
vec3 T = normalize(v_tangent - N * v_tangent * N);
vec3 B = cross(N, T);
mat3 TBN = mat3(T,B,N);

const float flame_engulf = 12.09;
void main(void)
{
    bool bMask = texture(Mask, v_texcoord).r > 0.8;
    vec3 bright_color;
    float NdotD = dot(windDir,N);
    if(bFire && NdotD > 0)
    {
        vec3 fire_base = vec3(0.585,0,0);
        vec3 fire_mod = vec3(0.975,0.553,0);

        float base = 0.5 * (NdotD + 1);
        float p = pow(base, flame_engulf);
        float depth = -v_depth.x/v_depth.y/zFar * 20;
        float alpha = clamp(1 - pow((normalize(cameraPos - v_worldPos) * TBN).z, 5) * clamp(NdotD,0,1),0,1);

        vec3 fire_color = mix(fire_base,fire_mod,alpha) * p;// * clamp(depth,0,1);
       //  vec3 fire_color = texture(fire,v_texcoord).xyz;
        if(color == vec3(-1,-1,-1))
        {
            bright_color = texture(albedo,v_texcoord).xyz;
        }
        else
        {
            bright_color = color;
        }
        fragColor = vec4(mix(bright_color,fire_color,NdotD),1);
        //fragColor = vec4(depth,depth,depth,1);
    }

    else if(bBloom)
    {
        if(color == vec3(-1,-1,-1))
        {
            bright_color = texture(albedo,v_texcoord).xyz;
        }
        else
        {
            bright_color = color;
        }
        if(bright_color.r > 0.95 || bright_color.b > 0.8 || bright_color.g > 0.8)
        {
            fragColor = vec4(bright_color,1);
        }
        else
        {
             fragColor = vec4(0,0,0,1);
        }
    }
    else
    {
        fragColor = vec4(0,0,0,1);
    }
}
