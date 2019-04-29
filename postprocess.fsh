#version 450 core

uniform sampler2D Light;
uniform int ScreenX;
uniform int ScreenY;
varying vec2 v_texcoord;

layout(location = 0) out vec4 LightTex;
void main(void)
{
    const int size = 7;


    float gauss[] = float[]
    (
        0.00000067, 0.00002292, 0.00019117, 0.00038771, 0.00019117, 0.00002292, 0.00000067,
        0.00002292, 0.00078633, 0.00655965, 0.01330373, 0.00655965, 0.00078633, 0.00002292,
        0.00019117, 0.00655965, 0.05472157, 0.11098164, 0.05472157, 0.00655965, 0.00019117,
        0.00038771, 0.01330373, 0.11098164, 0.22508352, 0.11098164, 0.01330373, 0.00038771,
        0.00019117, 0.00655965, 0.05472157, 0.11098164, 0.05472157, 0.00655965, 0.00019117,
        0.00002292, 0.00078633, 0.00655965, 0.01330373, 0.00655965, 0.00078633, 0.00002292,
        0.00000067, 0.00002292, 0.00019117, 0.00038771, 0.00019117, 0.00002292, 0.00000067
    );


   vec3 finalColor = vec3(0,0,0);

   int idx = 0;
   for(int i = -3;i <= 3;i++)
   {
       for(int j = -3; j <= 3;j++)
       {
           vec2 uv = v_texcoord + vec2(1.0 * i /ScreenX, 1.0 * j /ScreenY);
           vec3 color = texture2D(Light, uv).xyz;
           float weight = gauss[idx];
           finalColor = finalColor + weight * color;
           idx++;
       }
   }

   LightTex = vec4(finalColor, 1);
}
