#version 450 core
#extension GL_NV_shadow_samplers_cube : enable
in vec2 v_texcoord;

uniform vec3 cameraPos;
uniform sampler2D NormalAndDepth;
uniform sampler2D Param;
uniform sampler2D Color;
uniform sampler2D ShadowMap;
uniform sampler2D brdfLUT;
uniform sampler2D Snow_D;
uniform sampler2D Snow_N;
uniform sampler2D SnowDepth;
uniform samplerCube prefilterMap;
uniform samplerCube irradianceMap;
uniform samplerCube Cloud3DNoiseTextureA;
uniform mat4 Inverse_ViewMatrix;
uniform mat4 ViewMatrix;
uniform mat4 SnowMatrix;
uniform mat4 OrthoMatrix;
uniform vec3 lightPos[10];
uniform float zFar;
uniform float zNear;
uniform float fTime;
uniform int ScreenX;
uniform int ScreenY;
uniform vec3 lightPosition;

uniform float     thickness;
uniform float     mixRatio;
uniform float     brightness;

uniform mat4 ProjectMatrix;
uniform float Kq[10];

uniform vec3 lightColor[10];
uniform bool bHDR;
uniform bool bGamma;
uniform bool bSnow;
uniform float hdrExp;

uniform mat4 LightMatrix;

varying vec2 farPlanePos;

//    Param.x = rough;
//    Param.y = metal;
//    Param.z = ao;
float PI = 3.14159;
vec3 sun_color = vec3(1.0,0.9,0.1);
vec3 sky_color = vec3(0.4,0.5,0.6);
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}


vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}

float GetShadow(vec4 worldPos)
{
    float fShadow = 0.0;
    vec4 lightPos = (LightMatrix * (worldPos));
    float fDistance = -lightPos.z / zFar;
    lightPos = ProjectMatrix * lightPos;

    vec2 uv = lightPos.xy / lightPos.w * 0.5 + vec2(0.5, 0.5);

    uv.x = clamp(uv.x, 0, 1);
    uv.y = clamp(uv.y, 0, 1);

    float offset = 0.001;

    for(int i = -1; i <= 1; i++)
    {
        for(int j = -1; j <= 1; j++)
        {
            float fDistanceMap = texture2D(ShadowMap, uv + vec2(1.0 * i / ScreenX, 1.0 * j / ScreenY)).x;
            fShadow += fDistance - offset > fDistanceMap ? 0.6 : 1.0;
        }
    }
    fShadow /= 9.0;

    return fShadow;
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

    float offset = 0.001;

    float distanceMap = texture2D(ShadowMap, uv).x;
    return fDistance - offset > distanceMap;
}

float GetSnowCoverRatio(vec4 worldPos)
{
    vec4 snowPos = OrthoMatrix * (SnowMatrix * worldPos);
    float fDistance = (snowPos.z + 1)/2;
    float fSnow = 0;
    vec2 uv = snowPos.xy / snowPos.w * 0.5 + vec2(0.5, 0.5);

    uv.x = clamp(uv.x, 0, 1);
    uv.y = clamp(uv.y, 0, 1);

    for(int i = -1; i <= 1; i++)
    {
        for(int j = -1; j <= 1; j++)
        {
            float fDistanceMap = texture2D(SnowDepth, uv + vec2(5.0 * i / ScreenX, 5.0 * j / ScreenY)).x;
            fSnow += fDistance - 0.001 > fDistanceMap ? 0 : 1.0;
        }
    }
    fSnow /= 9;
    return fSnow;
}

vec3 hash3( vec3 p )
{
    p = vec3(dot(p,vec3(127.1,311.7,419.5)),dot(p,vec3(269.5,183.3,253.5)),dot(p,vec3(269.5,183.3,200.5)));
    return fract(sin(p) * 18.5453);
}

float worley( vec3 pos )
{
    vec3 n = floor(pos);
    vec3 f = fract(pos);

    float minDistance = 1000;
    for (int i = -1; i < 2; ++i)
    {
        for (int j = -1; j < 2; ++j)
        {
            for (int k = -1; k < 2; ++k)
            {
                vec3 gridPos = vec3( float(i), float(j), float(k) );
                vec3 offsetPos = hash3( n + gridPos );
                vec3 r = gridPos + offsetPos - f;
                float distance = dot( r, r );
                if( distance < minDistance )
                {
                    minDistance = distance;
                }
            }
        }
    }

    return minDistance;
}

float hash(vec3 p)
{
    p  = fract( p*0.3183099+.1 );
        p *= 17.0;
    return fract( p.x*p.y*p.z*(p.x+p.y+p.z) );
}

float noise( in vec3 x )
{
    vec3 p = floor(x);
    vec3 f = fract(x);
    f = f*f*(3.0-2.0*f);

    return mix(mix(mix( hash(p+vec3(0,0,0)),
                        hash(p+vec3(1,0,0)),f.x),
                   mix( hash(p+vec3(0,1,0)),
                        hash(p+vec3(1,1,0)),f.x),f.y),
               mix(mix( hash(p+vec3(0,0,1)),
                        hash(p+vec3(1,0,1)),f.x),
                   mix( hash(p+vec3(0,1,1)),
                        hash(p+vec3(1,1,1)),f.x),f.y),f.z);
}

float fracNoise(vec3 pos, int octave)
{
    float noiseSum = 0.0;
    pos = 4.0 * pos;
    int count = 1;
    while(octave != 0)
    {
        noiseSum += 1.0 / count * noise(pos);
        pos = 2.0 * pos;
        count = 2 * count;
        octave--;
    }

   return noiseSum;
}


bool IsIdEqual(float id, float data)
{
    return abs(id * 100 - data) < 0.1;
}

float distance(vec3 p)
{
    return sqrt(p.x * p.x + p.y * p.y + p.z * p.z);
}

mat4 dither = mat4(
   0,       0.5,    0.125,  0.625,
   0.75,    0.25,   0.875,  0.375,
   0.1875,  0.6875, 0.0625, 0.5625,
   0.9375,  0.4375, 0.8125, 0.3125
);

float gauss[] = float[]
(
    0.00000067, 0.00002292, 0.00019117, 0.00038771, 0.00019117, 0.00002292, 0.00000067,
    0.00002292, 0.00078633, 0.00655965, 0.01330373, 0.00655965, 0.00078633, 0.00002292,
    0.00019117, 0.00655965, 0.05472157, 0.11098164, 0.05472157, 0.00655965, 0.00019117,
    0.00038771, 0.01330373, 0.11098164, 0.22508352, 0.11098164, 0.01330373, 0.00038771,
    0.00019117, 0.00655965, 0.05472157, 0.11098164, 0.05472157, 0.00655965, 0.00019117,
    0.00002292, 0.00078633, 0.00655965, 0.01330373, 0.00655965, 0.00078633, 0.00002292,
    0.00000067, 0.00002292, 0.00019117, 0.00038771, 0.00019117, 0.00002292, 0.00000067
);


vec3 GetGaussColor(vec2 uv)
{
    const int size = 7;

    vec3 finalColor = vec3(0,0,0);

    int idx = 0;
    for(int i = -3;i <= 3;i++)
    {
        for(int j = -3; j <= 3;j++)
        {
            vec2 offset_uv = uv + vec2(5.0 * i /ScreenX, 5.0 * j /ScreenY);
            vec3 color = texture2D(Color, offset_uv).xyz;
            float weight = gauss[idx++];
            finalColor = finalColor + weight * color;
        }
    }

    return finalColor;
}

vec3 UnpackNormal(vec3 n)
{
    vec3 N = vec3(0,1,0);
    vec3 T = vec3(1,0,0);
    vec3 B = cross(N, T);
    mat3 TBN = mat3(T,B,N);
    n = normalize(2 * n - 1);
    n = normalize(TBN * n);
    return n;
}

vec3 BlendAngleCorrectedNormals(vec3 baseNormal, vec3 additionalNormal)
{
    baseNormal.b += 1;
    additionalNormal *= vec3(-1, -1, 1);
    vec3 normal =  dot(baseNormal, additionalNormal) * baseNormal - baseNormal.b * additionalNormal;
    return normalize(normal);
}

vec3 ComputeWorldPos(float depth)
{
    vec4 pos = vec4(vec3(farPlanePos.x, farPlanePos.y,-zFar) * depth , 1);
    vec4 ret = Inverse_ViewMatrix * pos;
    return ret.xyz / ret.w;
}

float GetHeightDensity(float height)
{
    if(height>300) return 0;
    else if(height > 200) 1;
    return height/200;
}

float GetLocalDensity(vec3 pos)
{
    vec3 t = pos / 50;
    t.x += fTime;
    float c1 = fracNoise(t, 3);
    float c2 = worley(t);

    c2 = 1 - c2;

    float sampled_density = mix(c1,c2,0.7);

    sampled_density = clamp((sampled_density - 0.7) / (1 - 0.7),0,1) ;
    sampled_density *= GetHeightDensity(pos.y);

    return sampled_density;
}

void main(void)
{
    vec4 result = texture2D(NormalAndDepth, v_texcoord);
    vec4 param  = texture2D(Param, v_texcoord);
    float rough = param.x;
    float metal = param.y;
    float ao    = param.z;
    float id    = param.w;

    int sampleCoordX = int(mod((ScreenX * v_texcoord.x),4));
    int sampleCoordY = int(mod((ScreenY * v_texcoord.y),4));
    float offset = dither[sampleCoordX][sampleCoordY];

    // Volume Light
   float I = 0.0;
   //int plane_num = 300;
   //int sample_num = 10;
   //for(int j = 1;j <= sample_num ;j++)
   //{
   //    float z = 1.0 * (j + offset) / plane_num;
   //    vec3 pos = ComputeWorldPos(z);
   //    if(!IsInShadow(vec4(pos,1)))
   //    {
   //        vec3 lightDis = pos - vec3(5,30,5);
   //        vec3 viewDis = pos - cameraPos;
   //
   //        vec3 lightDir = normalize(lightDis);
   //        vec3 viewDir = normalize(viewDis);
   //
   //        float cosTheta = dot(lightDir,normalize(-vec3(5,30,5)));
   //
   //        float g = 0.1;
   //        float hg = 5.0/(4*3.14)* (1 - g*g)/ pow(1 + g*g -2 * g * max(dot(lightDir,-viewDir),0), 1.5);
   //        if(cosTheta > 0.98)
   //        {
   //            I += clamp(hg / sample_num, 0, 1);
   //        }
   //    }
   //    I = clamp(I,0,1);
   //}

    if(result.x != 1 || (result.x == 1 && result.y == 0.5 && result.z == 0.5))
    {
        vec3 normal = normalize(result.xyz * 2 - 1);
        float depth = result.w;

        vec3 farPos = vec3(farPlanePos.x, farPlanePos.y,-zFar);
        vec3 viewPos = farPos * depth;
        vec4 tmp = Inverse_ViewMatrix * vec4(viewPos, 1);
        vec3 worldPos =  tmp.xyz / tmp.w;

        vec3 viewDir = normalize(cameraPos - worldPos);
        vec3 albedo = texture2D(Color, v_texcoord).xyz;

        // Calculate Snow_D
        if(bSnow)
        {
            float coverRatio = GetSnowCoverRatio(vec4(worldPos,1));
            float snowRatio = max(dot(normal,vec3(0,1,0)),0);
            snowRatio = min(snowRatio * 1.2 /(snowRatio + 0.3),1);
          //  snowRatio *= coverRatio;
            vec3 snowColor = vec3(texture2D(Snow_D, vec2(worldPos.x,worldPos.z)/40));
            vec3 snowNormal = UnpackNormal(vec3(texture2D(Snow_N, vec2(worldPos.x,worldPos.z)/40)));
            albedo = mix(albedo,snowColor,snowRatio);
            normal = mix(normal,snowNormal,snowRatio);
        }
        // Ray-Matching(SSR)
        vec3 reflectDir = reflect(-viewDir, normal);
        vec3 viewReflectDir = mat3(ViewMatrix) * reflectDir;

        float eps = 0.001;
        vec3 last_pos = viewPos;

        vec3 irradiance = textureCube(irradianceMap, normal).rgb;
        vec3 reflectColor = vec3(0);
        if(IsIdEqual(id, 3) )
        {
            //albedo = mix(albedo,textureCube(CubeMap, reflectDir).xyz,0.2);
            float step = 0.2;
            int stepNum = 30;

            for(int i = 1 ;i <= stepNum;i++)
            {
                float delta = step * i + offset;
                vec3 pos = viewPos + viewReflectDir * delta;
                float d = -pos.z / zFar;
                tmp = ProjectMatrix * vec4(pos, 1);
                vec3 screenPos = tmp.xyz / tmp.w;

                if(d < 0) break;
                vec2 uv = vec2(screenPos.x, screenPos.y) * 0.5 + vec2(0.5, 0.5);
                if(uv.x > 0 && uv.x < 1 && uv.y > 0 && uv.y < 1)
                {
                    float depth = texture2D(NormalAndDepth, uv).w;
                    if(depth == 1) break;
                    if(d > depth)
                    {
                        if(abs(d - depth) < eps)
                        {
                            float newId = texture2D(Param, uv).w;
                            if(!IsIdEqual(newId, 3))
                            {
                                reflectColor = texture2D(Color, uv).xyz;
                            }
                            break;
                        }

                         vec3 begin = last_pos;
                         vec3 end = pos;
                         vec3 currPos = (begin + end)/2;

                        int count = 0;
                        // binary search
                        while(true)
                        {
                            count++;
                            if(count > 3)
                            {
                                break;
                            }

                            d = -currPos.z / zFar;
                            tmp = ProjectMatrix * vec4(currPos,1);
                            screenPos = tmp.xyz / tmp.w;
                            uv = vec2(screenPos.x, screenPos.y) * 0.5 + vec2(0.5, 0.5);
                            depth = texture2D(NormalAndDepth, uv).w;
                             if(depth == 1) break;
                            if(abs(d - depth) < 0.3 * eps)
                            {
                                float newId = texture2D(Param, uv).w;
                                if(!IsIdEqual(newId, 3))
                                {
                                    reflectColor = texture2D(Color, uv).xyz;
                                }
                                break;
                            }
                            if(d > depth)
                            {
                                end = currPos;
                            }
                            else if(d < depth)
                            {
                                begin = currPos;
                            }
                            currPos = (begin + end)/2;
                        }
                    }
                    last_pos = pos;
                }
                else
                {
                    break;
                }
            }
        }

        vec3 F0 = vec3(0.04);
        F0 = mix(F0,albedo,metal);

        vec3 Lo = vec3(0.0);
        for(int i = 0; i < 5; i++)
        {
            vec3 dis = lightPos[i] - worldPos;
            float dis2 = dis.x * dis.x + dis.y * dis.y + dis.z * dis.z;

            float attenuation = 1.0 / (Kq[i] * dis2);

            vec3 radiance = lightColor[i] * attenuation;
            vec3 lightDir = normalize( lightPos[i] - worldPos );
            vec3 halfDir = normalize(lightDir + viewDir);

            float NDF = DistributionGGX(normal,halfDir,rough);
            float G   = GeometrySmith(normal,viewDir,lightDir,rough);
            vec3 F    = fresnelSchlickRoughness(max(dot(halfDir,viewDir),0.0),F0, rough);

            vec3 kS = F;
            vec3 kD = vec3(1.0) - kS;
            kD *= 1.0 - metal;

            vec3 nominator = NDF * G * F;
            float denominator = 4.0 * max(dot(normal,viewDir), 0.0) * max(dot(normal,lightDir),0.0) + 0.001;
            vec3 specular = nominator / denominator;

            float NdotL = max(dot(normal,lightDir), 0.0);
            Lo += (kD * albedo / PI + specular) * radiance * NdotL;
        }


        vec3 ambient;
        vec3 F = fresnelSchlickRoughness(max(dot(normal,viewDir),0.0), F0, rough);

        // specular
        float NDotV = dot(normal,viewDir);
        const float MAX_REFLECTION_LOD = 4.0;
        vec3 prefilteredColor = textureCube(prefilterMap, reflectDir).rgb;
        vec2 envBRDF = texture2D(brdfLUT, vec2(max(NDotV, 0.0), rough)).rg;
        vec3 specular = prefilteredColor * (F * envBRDF.x + envBRDF.y);

        // diffuse
        if(reflectColor == vec3(0))
        {
            vec3 kS = F;
            vec3 kD = vec3(1.0) - kS;
            kD *= 1.0 - metal;
            vec3 diffuse = irradiance * albedo;
            ambient = (kD * diffuse + specular) * ao;
        }
        else
        {
            ambient = reflectColor * ao;
        }


        float fShadow = GetShadow(vec4(worldPos,1));
        vec3 color = ambient + Lo ;

        if(bHDR)
        {
            // 曝光色调映射
            color = vec3(1.0) - exp(-color * hdrExp);
           // color = color / (color + vec3(1.0));
        }
        if(bGamma)
        {
            // Gamma校正
            const float gamma = 2.2;
            color = pow(color, vec3(1.0 / gamma));

        }

        color = color * fShadow;
        gl_FragColor = vec4(color + I * sun_color, 1.0);
        // gl_FragColor = vec4(depth,depth,depth, 1);

    }
    else
    {
        // texture2D(Color, v_texcoord).xyz
        gl_FragColor = vec4(texture2D(Color, v_texcoord).xyz , 1);
      //  gl_FragColor = vec4(sky_color + I * sun_color, 1);
    }
//else
//{
//    float density = 0.0;
//
//    vec3 beginPos;
//    beginPos.x = result.t;
//    beginPos.y = result.z;
//    beginPos.z = result.w;
//    beginPos = 2 * beginPos - 1;
//    beginPos *= 200;
//
//    vec3 step = 1.0 * normalize(beginPos - cameraPos);
//    bool bInit = false;
//    vec3 cloud_color = vec3(0);
//
//    int stepNum = 16;
//    for(int i = 0; i < stepNum; i++)
//    {
//        vec3 pos = beginPos + i * step;
//
//
//        float sampled_density = GetLocalDensity(pos);
//        density += sampled_density / stepNum;
//
//
//     vec3 lightDir = pos - vec3(5,230,5);
//     vec3 viewDir = pos - cameraPos;
//     float cos_angle = dot(normalize(lightDir), normalize(-viewDir));
//     float inG = 0.3;
//     float hg = ((1.0 - inG * inG) / pow((1.0 + inG * inG - 2.0 * inG * cos_angle), 3.0/2.0))
//         / 4.0 * 3.1415;
//
//        float powder = (1 - exp(-2 * density));
//        float beers = exp(-density);
//        cloud_color += vec3(1,1,1) * hg * 2 * beers * powder;
//    }
//
//    cloud_color = cloud_color / (0.5 + cloud_color);
//    if(density < 0.7) density = density/(density + 0.1) * 0.7;
//
//    gl_FragColor = vec4(mix(sky_color,cloud_color,density),1);
//}
}
