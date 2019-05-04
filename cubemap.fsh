#version 450 core
uniform sampler2D equirectangularMap;
in vec3 localPos;
out vec4 fragColor;

const vec2 invAtan = vec2(0.1591, 0.3183);

vec2 SampleSphericalMap(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}
const float PI = 3.14159265359;
void main()
{
    vec2 uv = SampleSphericalMap(normalize(localPos)); // make sure to normalize localPos
    vec3 color = texture(equirectangularMap, uv).rgb;

    fragColor = vec4(color, 1.0);
}
