#version 460

layout (location = 0) out vec4 FragColor;
in vec3 fNormal;
in vec3 fPosition;


uniform struct LightInfo
{
   vec4 Position;
   vec3 La;
   vec3 L;
   vec3 Intensity;
} Light;

uniform struct MaterialInfo
{
   vec3 Kd;
   vec3 Ka;
   vec3 Ks;
   float Shininess;
}Material;

vec3 blinnPhong( vec3 position, vec3 n)
{
  vec3 diffuse=vec3(0), spec=vec3(0);
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



void main()
{
     FragColor=vec4(blinnPhong(fPosition, fNormal), 1.0);
}