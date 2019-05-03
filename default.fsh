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
uniform sampler2D Bloom;
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
            fShadow += fDistance - offset > fDistanceMap ? 0.8 : 1.0;
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


bool UseSSR(float id)
{
   return abs(100 * id - 1) < 0.01;
}

bool UseFire(float id)
{
    return abs(100 * id - 2) < 0.01;
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

vec2 g_v2TwelveKernelBase[] =
{
    {1.0,0.0},{0.5,0.866},{-0.5,0.866},
    {-1.0,0.0},{-0.5,-0.866},{0.5,-0.866},
    {1.5,0.866},{0, 1.732},{-1.5,0.866},
    {-1.5,0.866},{0,-1.732},{1.5,-0.866},
};

vec3 GetBlurColor(vec2 uv)
{
    int kernelNum = 12;
    vec4 v4Original = texture2D(Bloom, uv);

    vec2 v4ScreenSize = vec2(ScreenX,ScreenY) / 5;
    vec3 v3Blurred = vec3(0, 0, 0);
    for(int i = 0; i < kernelNum; i++)
    {
       vec2 v2Offset = vec2(g_v2TwelveKernelBase[i].x / v4ScreenSize.x, g_v2TwelveKernelBase[i].y / v4ScreenSize.y);
       vec4 v4Current = texture2D(Bloom, uv + v2Offset);
       v3Blurred += mix(v4Original.rgb, v4Current.rgb, v4Original.a);
   }
   return v3Blurred / kernelNum;
}

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

vec3 GetBloomColor(vec2 uv)
{
    const int size = 7;

    vec3 finalColor = vec3(0,0,0);
    vec3 max = vec3(0,0,0);
    int idx = 0;
    for(int i = -3;i <= 3;i++)
    {
        for(int j = -3; j <= 3;j++)
        {
            vec2 offset_uv = uv + vec2(5.0 * i /ScreenX, 5.0 * j /ScreenY);
            vec3 color = texture2D(Bloom, offset_uv).xyz;
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
float value_fractal(vec3 p)
{
    float f = 0.0;
    p = p * 4.0;
    f += 1.0000 * noise(p); p = 2.0 * p;
    f += 0.5000 * noise(p); p = 2.0 * p;
    f += 0.2500 * noise(p); p = 2.0 * p;
    f += 0.1250 * noise(p); p = 2.0 * p;
    f += 0.0625 * noise(p); p = 2.0 * p;

   return f/2;
}

float worley_fractal(vec3 p)
{
    float f = 0.0;
    p = p * 4.0;
    f += 1.0000 * worley(p); p = 2.0 * p;
    f += 0.5000 * worley(p); p = 2.0 * p;


   return f/2;
}

float GetHeightDensity(float height)
{
    if(height > 200) return 1;
    else if(height > 120) return (height-120)/80;
    return 0;
}

float GetCloudBaseShape(vec3 pos)
{
    pos = pos/200;
    pos.x += fTime/3;
    float c1 = value_fractal(pos);
    float c2 = worley(pos);

    c2 = 1 - c2;

    float shape = mix(c1,c2,0.5);
    shape = clamp((shape - 0.3) / (1 - 0.3),0,1) ;
    return shape;
}

float GetCloudDetailShape(vec3 pos)
{
    pos = pos/10;
    pos.x += fTime/3;

    float c = worley(pos);

    c = 1 - c;
   // c = clamp((c - thickness) / (1 - thickness),0,1) ;
    return c;
}


float GetCloudThickness(vec3 pos)
{
    vec3 t = vec3(pos.x, 0.0, pos.z)/50;

    t.x += fTime/3;

    float c1 = value_fractal(t);
    float c2 = worley(t);

    c2 = 1 - c2;

    float thick = mix(c1,c2,0.7);
    thick = clamp((thick - 0.6) / (1 - 0.6),0,1) ;

    return thick;
}

float GetCloudDensity(vec3 pos)
{
    float thick = GetCloudThickness(pos);
    float base_density = GetCloudBaseShape(pos);
    float detail_density = GetCloudDetailShape(pos);
    return thick * base_density * 1;
}

vec3 GetLocalLight(vec3 pos, float density, vec3 sunCol)
{
    sunCol *= 200.0f;
   float dist = abs(600 - pos.y);			//到光源的距离
   vec3 dir = vec3(0.0, 1.0, 0.0);
   vec3 light = sunCol/ dist;		//计算当地光

   float shadow = 1.0f;
   float shadowNum = 1.0;
   float step = 1 / shadowNum;

   for (float i = 0.5; i < shadowNum; i+=2)
   {
        float density = GetCloudDensity(pos+dir*i*step);
        shadow *= exp(-step*density*2);
   }
   light *= shadow;
   return light;
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
   if(result.xy != vec2(1,1))
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
        if(UseSSR(id) )
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
                            if(!UseSSR(newId))
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
                                if(!UseSSR(newId))
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

        vec3 gauss_color = GetBloomColor(v_texcoord);
        color = color * fShadow;
        gl_FragColor = vec4(color + I * sun_color + gauss_color, 1.0);
    }
    else if(result ==vec4(1,1,1,1))
    {
       vec3 gauss_color = GetBloomColor(v_texcoord);
        gl_FragColor = vec4(texture2D(Color, v_texcoord).xyz  + gauss_color, 1);
    }
    else
    {
        float density = 0.0;

        vec3 beginPos;
        beginPos.x = (result.z * 2 - 1);
        beginPos.z = (result.w * 2 - 1);

        beginPos.x *= 400;
        beginPos.z *= 400;
        beginPos.y = 400;

        vec3 cloud_color = vec3(0);

        float stepLength = 1;
        vec3 step = 20 * normalize(beginPos - cameraPos);

        int stepNum = 10;
        float transmittance = 1.0;
        vec3 light_color;
        for(int i = 0; i < stepNum; i++)
        {
            vec3 pos = beginPos + i * step;


            float sampled_density = GetCloudDensity(pos);//* GetHeightDensity(pos.y);
            density += sampled_density;

            vec3 lightDir = pos - vec3(0,400,-100);
            vec3 viewDir = pos - cameraPos;
            float cos_angle = dot(normalize(lightDir), normalize(viewDir));
            float inG = 0.2;
            float hg = ((1.0 - inG * inG) / pow((1.0 + inG * inG - 2.0 * inG * cos_angle), 3.0/2.0))
             / 4.0 * 3.1415;

            float powder = (1.0f - exp(-sampled_density*1));
            vec3 localCol = GetLocalLight(pos,sampled_density,vec3(1,0.8,0.9)) * powder;
          //  vec3 localCol = vec3(1,1,1) * (1.0f - exp(-sampled_density*2));
            cloud_color += (localCol * hg) * transmittance ;
            transmittance *= exp(-2*sampled_density);
          //  light_color += vec3(1,0,0) * hg * beers;
            //cloud_color +=  vec3(1,1,1)* clamp(2 * beers * powder,0,1);//+ sun_color* hg * powder   ;// + vec3(1,0,0) * hg * beers;
        }

        cloud_color += sky_color * transmittance;

        vec3 fogColor = vec3(1.0, 1.00, 0.94);
        float factor = clamp(0, 1, (distance(cameraPos, beginPos))/ 1200.0);
        cloud_color = mix(cloud_color, fogColor, factor);

        density = density/(density + 1);
        vec3 gauss_color = GetBloomColor(v_texcoord);
        gl_FragColor = vec4(mix(sky_color,cloud_color,density) + gauss_color,1);

    }
}

