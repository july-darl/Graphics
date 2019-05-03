#version 450 core
uniform sampler2D brick_N;
uniform sampler2D brick_D;
uniform sampler2D brick_M;
uniform sampler2D shadowMap;

uniform int type;
uniform float zFar;
uniform int ScreenX;
uniform int ScreenY;

varying vec3 v_normal;
varying vec2 v_texcoord;
varying vec3 ViewDir;
varying vec3 lightDir;
varying vec4 lightPos;

layout(location = 0) out vec4 Color;
float GetShadow()
{
    float fShadow = 0.0;
    float fDistance = lightPos.z / zFar;

    vec2 uv = lightPos.xy / lightPos.w * 0.5 + vec2(0.5, 0.5);

    uv.x = clamp(uv.x, 0, 1);
    uv.y = clamp(uv.y, 0, 1);

    float offset = 0.5/zFar;

    for(int i = -1; i <= 1; i++)
    {
        for(int j = -1; j <= 1; j++)
        {
            vec4 distance = texture2D(shadowMap, uv + vec2(1.0 * i / ScreenX, 1.0 * j / ScreenY));
            float fDistanceMap = distance.r + distance.g/65536.0 + distance.b /(65536.0*65536.0);
            fShadow += fDistance - offset > fDistanceMap ? 0.4 : 0.8;
        }
    }
    fShadow /= 9.0;

    return fShadow;
}

vec3 UnpackNormal(vec3 normal)
{
    normal = normalize(2 * normal - 1);
    return normal;
}

vec2 Pallax()
{
    float h = texture2D(brick_D, v_texcoord).a;
    return v_texcoord + ViewDir.xy / ViewDir.z * (h * 0.002 - 0.001);
}

vec2 SteepPallax()
{
   float layer = mix(2, 1, abs(dot(vec3(0, 0, 1), ViewDir)));

   vec2 deltaUV = 0.02 * ViewDir.xy / ViewDir.z  / layer;

   vec2 uv = v_texcoord;
   float h = texture2D(brick_D, uv).a;
   float curH = 0.0;
   float deltaH = 1.0 / layer;

   while(h > curH)
   {
       curH += deltaH;
       uv += deltaUV ;
       h = texture2D(brick_D, uv).a;
   }

   return uv;
}

void main()
{
   vec2 uv = Pallax();

   vec3 normal = vec3(texture2D(brick_N, uv));
   normal = UnpackNormal(normal);

   float ao = vec3(texture2D(brick_M, uv)).g;
   float rough = vec3(texture2D(brick_M, uv)).r;

   float diffuse = clamp(dot(normal, lightDir), 0, 1);
   float ambient = 0.5;
   vec3 reflectDir = normalize(reflect(-lightDir,v_normal));

   vec3 color = vec3(texture2D(brick_D, uv));
   float specular = pow(clamp(dot(reflectDir,ViewDir),0,1),100 * rough);

   float shadow = GetShadow();
   vec3 finalColor = color * ( (specular + diffuse) * shadow + ambient * ao);

   Color = vec4(finalColor,1);

}
