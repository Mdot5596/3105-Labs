#version 460

layout(location = 0) in vec3 VertexPosition; // Position of the vertex
layout(location = 1) in vec3 VertexNormal;   // Normal vector of the vertex
layout(location = 2) in vec3 VertexColor;    // Per-vertex color

flat out vec3 LightIntensity; 
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

uniform struct LightInfo
{

  vec4 Position; //Light Position in eye coords
  vec3 La;       //Ambient light intensity
  vec3 L;

} lights[3];

uniform struct MaterialInfo
{
vec3 Ka; //ambient reflectivity
vec3 Kd; //Diffuse reflectivity
vec3 Ks; //Specular reflectivity
float Shininess;

} Material;

vec3 phongModel(int light, vec3 position, vec3 n)
{
    // Normalize the normal vector
    n = normalize(n);

    // Step 1: Calculate Ambient Light
    vec3 ambient = Material.Ka * lights[light].La;

    // Step 2: Calculate Diffuse Light
    vec3 s = normalize(vec3(lights[light].Position) - position); // Light direction
    float sDotN = max(dot(s, n), 0.0); // Diffuse factor
    vec3 diffuse = Material.Kd * sDotN;

    // Step 3: Calculate Specular Light
    vec3 spec = vec3(0.0);
    if (sDotN > 0.0)
    {
        vec3 v = normalize(-position); // View direction
        vec3 r = reflect(-s, n); // Reflection vector
        spec = Material.Ks * pow(max(dot(r, v), 0.0), Material.Shininess);
    }

    // Step 4: Combine all components
    return ambient + lights[light].L * (diffuse + spec);
}
