#version 460

layout(location = 0) in vec3 VertexPosition; 
layout(location = 1) in vec3 VertexNormal;   
//layout (location = 2) in vec2 VertexTexCoord; 

out vec3 EyePosition;
out vec3 EyeNormal;
out vec4 ProjTexCoord;

uniform mat4 ModelViewMatrix;    
uniform mat3 NormalMatrix;      
uniform mat4 ModelMatrix;
uniform mat4 MVP;                

uniform mat4 ProjectorMatrix;

void main() 
{

   EyePosition =(ModelViewMatrix*vec4(VertexPosition,1.0)).xyz;
   EyeNormal = normalize(NormalMatrix*VertexNormal);
   ProjTexCoord=ProjectorMatrix*(ModelMatrix*vec4(VertexPosition,1.0));

   gl_Position = MVP*vec4(VertexPosition,1.0);

}
