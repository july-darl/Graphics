#version 450 core

varying vec2 v_texcoord;
uniform float zFar;
uniform sampler2D Color;
uniform sampler2D Normal;

uniform int ScreenX;
uniform int ScreenY;

float sobel_x[] =
{
    -1,-2,-1,
    0 , 0, 0,
    1 , 2, 1,
};

float sobel_y[] =
{
    -1, 0, 1,
    -2, 0, 2,
    -1, 0, 1,
};


void main(void)
{
    vec4 curNormalX = vec4(0, 0, 0, 0);
    int k = 0;
    for(int i = -1; i <=1 ;i++)
    {
        for(int j = -1; j <= 1;j++)
        {
            vec4 ret =  texture2D(Normal, v_texcoord + vec2(1.0 * i / ScreenX, 1.0 * j / ScreenY));
            curNormalX += sobel_x[k++] * ret;
        }
    }
    k = 0;
    vec4 curNormalY = vec4(0, 0, 0, 0);
    for(int i = -1; i <=1 ;i++)
    {
        for(int j = -1; j <= 1;j++)
        {
            curNormalY += sobel_y[k++] * texture2D(Normal, v_texcoord + vec2(1.0 * i / ScreenX, 1.0 * j / ScreenY));
        }
    }


    vec4 size = sqrt(curNormalX * curNormalX + curNormalY * curNormalY);

   gl_FragColor = texture2D(Color, v_texcoord);
   float threshold = 100.0/zFar;
   if(size.x > threshold || size.y > threshold || size.z > threshold || size.w > 1.0/zFar)
   {
       gl_FragColor = vec4(0,0,0,1);
    }
}
