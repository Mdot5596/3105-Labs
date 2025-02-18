#version 460

layout(location = 0) in vec3 VertexPosition; // Position of the vertex
layout(location = 1) in vec3 VertexNormal;   // Normal vector of the vertex
layout(location = 2) in vec3 VertexColor;    // Per-vertex color

out vec3 LightIntensity; 
out vec3 Position;   // Eye-space position
out vec3 Normal;     // Eye-space normal
out vec3 Color;      // Pass vertex color
out vec3 ViewDir;    // Direction to the camera (needed for specular)

uniform vec4 LightPosition;     // Light position in view space
uniform vec3 Kd;                // Diffuse reflectivity
uniform vec3 Ld;                // Light intensity
uniform vec3 Ks;                // Specular reflectivity
uniform float Shininess;         // Shininess factor
uniform mat4 ModelViewMatrix;    // Model-View matrix
uniform mat4 MVP;                // Model-View-Projection matrix
uniform mat3 NormalMatrix;       // Normal transformation matrix

void getCamSpaceValues(out vec3 norm, out vec3 position)
{
    norm = normalize(NormalMatrix * VertexNormal);
    position = (ModelViewMatrix * vec4(VertexPosition, 1.0)).xyz;
}

void main() 
{
    Color = VertexColor;

    // Compute normal and position in eye space
    getCamSpaceValues(Normal, Position);

    // Compute view direction (from fragment to camera)
    ViewDir = normalize(-Position); 

    // Transform vertex to clip space
    gl_Position = MVP * vec4(VertexPosition, 1.0);

    // Compute the light direction
    vec3 lightDir = normalize(vec3(LightPosition) - Position);

    // Diffuse shading calculation
    float diff = max(dot(Normal, lightDir), 0.0);

    // Pass the diffuse component to the fragment shader
    LightIntensity = Ld * Kd * diff;
}
