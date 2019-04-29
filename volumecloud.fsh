#version 450

uniform sampler2D color;
uniform sampler2D perlinNoise;
uniform float     zFar;
uniform vec3      cameraPos;
uniform mat4      ProjectMatrix;
uniform mat4      ViewMatrix;
uniform float     time;

uniform float     thickness;
uniform float     mixRatio;
uniform float     brightness;

varying vec3      v_worldPos;

layout(location = 0) out vec4 NormalAndDepth;
layout(location = 1) out vec4 Color;
layout(location = 2) out vec4 Param;

vec3 hash3( vec3 p )
{
    p = vec3(dot(p,vec3(127.1,311.7,419.5)),dot(p,vec3(269.5,183.3,253.5)),dot(p,vec3(269.5,183.3,259.5)));
    return fract(sin(p) * 18.5453);
}

float worley( vec3 pos )
{
    vec3 n = floor(pos);
    vec3 f = fract(pos);

    float minDistance = 10;
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


float HenyeyGreenstein(vec3 inLightVector, vec3 inViewVector, float inG, float density,float p)
{
    float cos_angle = dot(normalize(inLightVector), normalize(inViewVector));
    float beer = 2.0 * exp(-density * p) * (1 - exp(-2 * density));
    float hg = ((1.0 - inG * inG) / pow((1.0 + inG * inG - 2.0 * inG * cos_angle), 3.0/2.0))
        / 4.0 * 3.1415;
    return beer * hg;
}

mat4 dither = mat4(
   0,       0.5,    0.125,  0.625,
   0.75,    0.25,   0.875,  0.375,
   0.1875,  0.6875, 0.0625, 0.5625,
   0.9375,  0.4375, 0.8125, 0.3125
);

vec3 sun_color = vec3(241.0/255,141.0/255,0.0/255);

float GetHeightDensity(float height)
{
    if(height>300) return 0;
    else if(height > 200) 1;
    return height/200;
}

float GetCloudBaseShape(vec3 pos)
{
    pos = pos/50;
    pos.x += time/3;
    float c1 = value_fractal(pos);
    float c2 = worley(pos);

    c2 = 1 - c2;

    float shape = mix(c1,c2,mixRatio);
   // shape = clamp((shape - thickness) / (1 - thickness),0,1) ;
    return shape;
}

float GetCloudDetailShape(vec3 pos)
{
    pos = pos/20;
    pos.x += time/3;

    float c = worley_fractal(pos);

    c = 1 - c;
   // c = clamp((c - thickness) / (1 - thickness),0,1) ;
    return c;
}


float GetCloudThickness(vec3 pos)
{
    vec3 t = vec3(pos.x, 0.0, pos.z)/20;

    t.x += time/3;

    float c1 = value_fractal(t);
    float c2 = worley(t);

    c2 = 1 - c2;

    float thick = mix(c1,c2,mixRatio);
    thick = clamp((thick - thickness) / (1 - thickness),0,1) ;

    return thick;
}

float GetCloudDensity(vec3 pos)
{
    float thick = GetCloudThickness(pos);
    float base_density = GetCloudBaseShape(pos);
    float detail_density = GetCloudDetailShape(pos);
    return thick * base_density * detail_density;
}

vec3 GetLocalLight(vec3 pos)
{//
 // float dist = distance(pos,vec3(0,80,0));		//到光源的距离
 // vec3 dir = vec3(0.0, 1.0, 0.0);
 // vec3 light = vec3(1,1,1) * dist;		//计算当地光
 //
 // float shadow = 1.0f;
 // float shadowNum = 2.0;
 // float step = 1.0/ shadowNum;
 //     //考虑阴影
 // for (float i = 0.5; i < shadowNum; i++)
 // {
 //     float density = GetLocalDensity(pos + dir*i*step);
 //     shadow *= exp(-step*density);
 // }
 // light *= shadow;
    return vec3(1,1,1);
}



vec3 ToneMapping (vec3 x)
{
    const float A = 0.15;
    const float B = 0.50;
    const float C = 0.10;
    const float D = 0.20;
    const float E = 0.02;
    const float F = 0.30;
    return ((x*(A*x + C * B) + D * E) / (x*(A*x + B) + D * F)) - E / F;
}

vec3 sky_color = vec3(0.4,0.5,0.6);
void main(void)
{

    float density = 0.0;
    vec3 cloud_color = vec3(0);
    vec3 worldPos = v_worldPos;
    vec3 step = 0.3 *normalize(v_worldPos - cameraPos);

    worldPos.y += 200;
    int stepNum = 30;
    float beers = 1;

    for(int i = 0; i < stepNum; i++)
    {
        vec3 pos = worldPos + i * step;
        if(pos.y > 230) break;
        float sampled_density = GetCloudDensity(pos);
        density += sampled_density;

        vec3 lightDir = pos - vec3(0,200,0);
        vec3 viewDir = pos - cameraPos;
        float cos_angle = dot(normalize(lightDir), normalize(-viewDir));
        float inG = 0.2;
        float hg = ((1.0 - inG * inG) / pow((1.0 + inG * inG - 2.0 * inG * cos_angle), 3.0/2.0))
         / 4.0 * 3.1415;

        float powder = (1 - exp(-2 * density));
        float beers = exp(-0.5 * density);
        cloud_color +=  vec3(1,1,1) * beers;//2 * beers * powder + vec3(1,0,0) * hg * beers;


        if(density > 5) break;
        // if(cloud_color.x >= 1) break;
        // * powder * hg / 2;
        //if(cloud_color > 1)
        //{
        //    cloud_color = 1;
        //    break;
        //}
    }

    //cloud_color = ToneMapping(cloud_color);
    cloud_color = cloud_color / (1 + cloud_color);
    density = density/(density + 1) * 1;


    Color.xyz = mix(sky_color,(cloud_color),density);
  //
  //float thick = noise_sum(vec3(worldPos.x / 70, 0.0, worldPos.z / 70));//noise_sum(vec3(worldPos.x / 50, 0.0, worldPos.z / 50)) / 2;
  //if(thick > 1)
  //    Color.xyz = vec3(1,0,0);
  //else if(thick < 0)
  //    Color.xyz = vec3(1,1,0);
  //else
  //Color.xyz = vec3(thick,thick,thick);

    NormalAndDepth = vec4(1,1,1,1);
}


