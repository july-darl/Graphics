#version 450 core
attribute vec4 a_position;
attribute vec3 a_normal;
attribute vec2 a_texcoord;

varying vec2 v_texcoord;

void main()
{
   //gl_Position = ModelMatrix * a_position;
   //vec3 worldPos = vec3(gl_Position);
   //gl_Position = ViewMatrix * gl_Position;
   //gl_Position = ProjectMatrix * gl_Position;
   //
   //ViewDir = normalize( (cameraPos - worldPos) );
   //lightDir = normalize( (LightLocation - worldPos));
   //v_texcoord = a_texcoord;
   //v_normal = mat3(IT_ModelMatrix) * a_normal;
   //v_depth = worldPos.z;
    gl_Position = a_position;
    v_texcoord = a_texcoord;
}
