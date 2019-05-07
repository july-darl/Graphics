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
uniform float zNear;
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

const float flame_engulf = 1.09;
void main(void)
{
    bool bMask = texture(Mask, v_texcoord).r > 0.8;

    float NdotD = dot(normalize(windDir),normalize(v_normal));
    if(bFire)
    {
        vec3 fire_base = vec3(0.585,0,0); // red
        vec3 fire_mod = vec3(0.975,0.553,0); // yellow

        float base = 0.5 * (NdotD + 1);
        float p = pow(base, flame_engulf);
        float depth = (-v_depth.x/v_depth.y )/zFar * 20;

        vec3 viewDir = normalize(cameraPos - v_worldPos);
        vec3 tangent_viewDir = viewDir * TBN;

        float z = normalize(tangent_viewDir).z;
       // z = (z + 1)/2;
        float t = clamp(1 - pow(z, 5),0,1);
        float alpha =  t * clamp(NdotD,0,1);

        vec3 fire_color = mix(fire_mod,fire_base,alpha) * p * clamp(depth,0,1);
       //  vec3 fire_color = texture(fire,v_texcoord).xyz * p* clamp(depth,0,1);


        fire_color = vec3(1.0) - exp(-fire_color * 0.6);
        const float gamma = 2.2;
        fire_color = pow(fire_color, vec3(1.0 / gamma));


        fragColor = vec4(fire_color,1);

       // fragColor = vec4(mix(bright_color,fire_color,NdotD),1);
      //  fragColor = vec4(base,base,base,1);
    }

    else if(bBloom)
    {
        vec3 bright_color;
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
