#version 460

in vec3 GPosition;
in vec3 GNormal;
noperspective in vec3 GEdgeDistance;


layout (location = 0) out vec4 FragColor;

uniform struct LightInfo {
    vec4 Position;
    vec3 La;
    vec3 L;
    vec3 Intensity;

} Light; 


uniform struct MaterialInfo{
   vec3 Kd;
   vec3 Ka;
   vec3 Ks;
   float Shininess;

}Material;

uniform struct LineInfo
{
  float Width;
  vec4 Color;

}Line;


vec3 blinnPhong ( vec3 position, vec3 n)
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
   vec4 color=vec4(blinnPhong(GPosition, GNormal), 1.0);
   float d=min(GEdgeDistance.x, GEdgeDistance.y);
   d=min(d, GEdgeDistance.z);

   float mixVal;
   if (d<Line.Width-1) mixVal=1.0;
   else if (d>Line.Width+1) mixVal=0.0;
   else {
     float x=d-(Line.Width-1);
     mixVal=exp2(-2.0*(x*x));
   }
   FragColor=mix(color, Line.Color, mixVal);
}