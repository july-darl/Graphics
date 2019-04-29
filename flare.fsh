#version 450
uniform sampler2D LightTex;
uniform vec3 Color;
varying vec2 v_texcoord;

void main(void)
{
    vec4 color = texture2D(LightTex, v_texcoord);
    float r = color.r;
    if(color.r > 0.5f)
    {
         discard;
    }
    else
    {
        gl_FragColor = vec4(Color,0.5f);
    }
}
