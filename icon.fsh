uniform sampler2D LightTex;
in vec2 v_texcoord;
out vec4 fragColor;

void main(void)
{
    vec4 color = texture2D(LightTex, v_texcoord);
   // if(color.w < 0.1)
   //     discard;
    fragColor = color;
}
