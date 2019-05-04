#version 450 core
#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif


uniform sampler2D tex;
in vec2 v_texcoord;

void main(void)
{
    gl_FragColor = texture(tex, v_texcoord);
}
