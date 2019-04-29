#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif
uniform samplerCube cubemap;
uniform mat4 ViewMatrix;
uniform int type;

uniform vec3 cameraPos;
varying vec3 worldPos;
varying vec3 v_normal;

float saturate(float data)
{
    if(data < 0.0)
    {
        return 0.0;
    }
    else if(data > 1.0)
    {
        return 1.0;
    }
    return data;
}

void main()
{
    vec3 N = normalize(v_normal);
    vec3 I = normalize(worldPos - cameraPos);
    vec3 R = reflect(I, N);

    vec4 reflectedColor = textureCube(cubemap, R);
    vec3 T = refract(I, N, 1.1);

    vec4 refractedColor = textureCube(cubemap, T);

    if(type == 0)
    {
        gl_FragColor = reflectedColor;
    }
    else if(type == 1)
    {
       gl_FragColor = refractedColor;
    }
    else if(type == 2)
    {
        float IdotN = dot(-I, N);
       // reflectedColor = vec4(1,0,0,1);
       // refractedColor = vec4(1,1,1,1);
        float fresnel = 0.1 + 0.8 * pow(1.0 - IdotN, 1.0);
        gl_FragColor = mix(refractedColor, reflectedColor, fresnel);
    }

}
