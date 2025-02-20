#version 460

layout(location = 0) in vec3 VertexPosition; // Position of the vertex
layout(location = 1) in vec3 VertexNormal;   // Normal vector of the vertex

out vec3 Colour;

flat out vec3 LightIntensity; 

uniform struct LightInfo{
   vec4 Position;
   vec3 La;
   vec3 L;
}Lights[3];

uniform struct MaterialInfo{
   vec3 Kd;
   vec3 Ka;
   vec3 Ks;
   float Shininess;

}Material;


uniform mat4 ModelViewMatrix;    // Model-View matrix
uniform mat3 NormalMatrix;       // Normal transformation matrix
uniform mat4 ProjectionMatrix;
uniform mat4 MVP;                // Model-View-Projection matrix


vec3 phongModel ( int light, vec3 position, vec3 n)
{
    vec3 ambient=Lights[light].La*Material.Ka;

    vec3 s=normalize(vec3(Lights[light].Position.xyz)-position);
    float sDotN=max(dot(s,n),0.0);
    vec3 diffuse=Material.Kd*sDotN;

    vec3 spec=vec3(0.0);
    if (sDotN>0.0)
    {
    vec3 v=normalize(-position.xyz);
    vec3 r=reflect(-s,n);
    spec=Material.Ks*pow(max(dot(r,v),0.0),Material.Shininess);
    }
    return ambient+(diffuse+spec)*Lights[light].L;
}

void main() 
{
   vec3 n = normalize(NormalMatrix*VertexNormal);
   vec3 camCoords=(ModelViewMatrix*vec4(VertexPosition,1.0)).xyz;

   
   Colour=vec3(0.0);
   for (int i=0;i<3;i++)
    Colour+=phongModel(i, camCoords,n);

   gl_Position = MVP*vec4(VertexPosition,1.0);

 
}
