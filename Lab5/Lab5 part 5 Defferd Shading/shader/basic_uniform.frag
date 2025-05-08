#version 460

in vec3 Position;
in vec3 Normal;
in vec2 TexCoord;

layout (binding=0) uniform sampler2D PositionTex;
layout (binding=1) uniform sampler2D NormalTex;
layout (binding=2) uniform sampler2D ColorTex;

uniform int Pass;
const float Gamma=2.2f;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec3 PositionData;
layout (location = 2) out vec3 NormalData;
layout (location = 3) out vec3 ColorData;

uniform struct LightInfo {
   vec4 Position;
   vec3 La;
   vec3 L;
} Light; 


uniform struct MaterialInfo{
  vec3 Kd;
 //  vec3 Ka;
 //  vec3 Ks;
 // float Shininess;
}Material;

/*
vec3 blinnPhong ( vec3 position, vec3 n, )
{
    vec3 diffuse=vec3(0), spec=vec3(0);
  //  vec3 texColor=texture(RenderTex, TexCoord).rgb;
    vec3 ambient=Light.La*Material.Ka;
    vec3 s=normalize(Light.Position.xyz-position);
    float sDotN=max(dot(s,n),0.0);
    diffuse=Material.Kd*sDotN;
    if (sDotN>0.0)
     {
    vec3 v=normalize(-position.xyz);
    vec3 h=normalize(v+s);
    spec=Material.Ks*pow(max(dot(h,n),0.0),Material.Shininess);
      }
    return ambient+(diffuse+spec)*Light.L;
}
*/

vec3 diffuseModel(vec3 pos, vec3 norm, vec3 diff)
{
   vec3 s=normalize(vec3(Light.Position)-pos);
   float sDotN=max(dot(s, norm),0.0);
   return Light.L*diff*sDotN;
}

void pass1()
{
   PositionData=Position;
   NormalData=normalize(Normal);
   ColorData=Material.Kd;
}

void pass2()
{
   vec3 pos = vec3(texture(PositionTex, TexCoord));
   vec3 norm = vec3(texture(NormalTex, TexCoord));
   vec3 diffColor = vec3(texture(ColorTex, TexCoord));
   FragColor=vec4(pow(vec3(diffuseModel(pos,norm,diffColor)), vec3(1.0/Gamma)), 1.0);
}


void main()
{
   if (Pass==1) pass1();
  else if (Pass==2) pass2();

}