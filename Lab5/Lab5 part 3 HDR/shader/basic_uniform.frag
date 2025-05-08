#version 460

in vec3 Position;
in vec3 Normal;
in vec2 TexCoord;

layout (binding=0) uniform sampler2D hdrTex;

uniform int Pass;
uniform float AveLum;
uniform float Exposure=0.35;
uniform float White=0.928;
uniform bool DoToneMap=true;


layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec3 HdrColor;

uniform mat3 rgb2xyz=mat3
(
   0.4142564, 0.2126729, 0.0193339,
   0.3575761, 0.7151522, 0.1191920,
   0.1804375, 0.0721750, 0.9503041

);

uniform mat3 xyz2rgb=mat3
(
  3.2404542, -0.9692660, 0.0556434,
  -1.5371385, 1.8760108, -0.2040259,
  -0.4985314, 0.0415560, 1.0572252
);

uniform struct LightInfo {
   vec4 Position;
   vec3 La;
   vec3 L;
} Lights[3]; 


uniform struct MaterialInfo{
   vec3 Kd;
   vec3 Ka;
   vec3 Ks;
   float Shininess;
}Material;


vec3 blinnPhong ( vec3 position, vec3 n, int i)
{
    vec3 diffuse=vec3(0), spec=vec3(0);
  //  vec3 texColor=texture(RenderTex, TexCoord).rgb;
    vec3 ambient=Lights[i].La*Material.Ka;
    vec3 s=normalize(Lights[i].Position.xyz-position);
    float sDotN=max(dot(s,n),0.0);
    diffuse=Material.Kd*sDotN;
    if (sDotN>0.0)
     {
    vec3 v=normalize(-position.xyz);
    vec3 h=normalize(v+s);
    spec=Material.Ks*pow(max(dot(h,n),0.0),Material.Shininess);
      }
    return ambient+(diffuse+spec)*Lights[i].L;
}

void pass1()
{
 vec3 n=normalize(Normal);
 HdrColor=vec3(0.0);
 for(int i=0; i<3; i++)
 HdrColor+blinnPhong(Position, n, i);
}

void pass2()
{
  vec4 color=texture(hdrTex,TexCoord);
  vec3 xyzCol=rgb2xyz*vec3(color);
  float xyzSum=xyzCol.x+xyzCol.y+xyzCol.z;
  vec3 xyYCol=vec3(xyzCol.x/xyzSum, xyzCol.y/xyzSum, xyzCol.y);

  float L=(Exposure*xyYCol.z)/AveLum;
  L=(L*(1+L/(White*White)))/(1+L);
  xyzCol.x=(L*xyYCol.x)/xyYCol.y;
  xyzCol.y=L;
  xyzCol.z=(L*(1-xyYCol.x-xyYCol.y))/xyYCol.y;
  if (DoToneMap)
      FragColor=vec4(xyz2rgb*xyzCol, 1.0);
      else
      FragColor=color;
}



void main()
{
   if (Pass==1) pass1();
  else if (Pass==2) pass2();
}