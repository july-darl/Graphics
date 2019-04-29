uniform sampler2D LightTex;
varying vec2 v_texcoord;

void main(void)
{
    vec4 color = texture2D(LightTex, v_texcoord);
   // if(color.w < 0.1)
   //     discard;
    gl_FragColor = color;
}
