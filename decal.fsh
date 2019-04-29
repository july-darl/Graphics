#version 450

uniform sampler2D Bush;
uniform sampler2D NormalAndDepth;
uniform sampler2D RT_Color;
uniform mat4 worldToDecal;
uniform mat4 Inverse_ViewProjMatrix;
uniform float decalSize;

in vec2 v_texcoord;
in vec4 v_projPos;

layout(location = 0) out vec4 Normal;
layout(location = 1) out vec4 Color;

vec3 ComputeWorldPos(float depth)
{
    vec4 pos;
    pos.w = 1;
    pos.z = depth * 2 - 1;
    pos.xy = v_projPos.xy/v_projPos.w;

    vec4 ret = Inverse_ViewProjMatrix * pos;
    return ret.xyz/ret.w;
}

void main(void)
{
    vec2 samplerCoord = (v_projPos.xy/v_projPos.w + 1)/2;

    // sample the depth at the current fragment
    float pixelDepth = texture2D(NormalAndDepth, samplerCoord).w;

    // compute the fragment's world-space position
    vec3 worldPos = ComputeWorldPos(pixelDepth);

    // transform into decal space
    vec4 pos = worldToDecal * vec4(worldPos,1);
    vec3 decalPos = pos.xyz/pos.w;

    // get the world-space normal at the fragment position
    vec3 worldNormal = texture2D(NormalAndDepth,samplerCoord).xyz;
    worldNormal = worldNormal * 2 - 1;
    vec3 decalNormal = mat3(worldToDecal) * worldNormal;

    // use the xy position of the position for the texture lookup
    vec2 texcoord;
    float threshold = 0.1f;
  //  if(abs(dot(vec3(worldToDecal[2]), worldNormal)) < threshold)
    {
        texcoord = (decalPos.xy + decalPos.z * decalNormal.xy)/decalSize + 0.5;
    }
  //  else
  //  {
  //      texcoord = decalPos.xy / decalSize + 0.5;
  //  }

    vec4 color = texture2D(Bush, texcoord);
    if(color.a == 1)
    {
        Color = color;//vec4(worldNormal,1);
        Normal = vec4(1,1,1,1);
    }
    else
    {
        Color = vec4(1,0,0,1);
        discard;
    }
    // Color = vec4(1,0,0,1);
}
