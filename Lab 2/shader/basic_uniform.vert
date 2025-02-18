#version 460

layout(location = 0) in vec3 VertexPosition; // Position of the vertex
layout(location = 1) in vec3 VertexNormal;   // Normal vector of the vertex
layout (location = 1) in vec3 VertexColor;

out vec3 LightIntensity; 
out vec3 Color;

uniform vec4 LightPosition;     // Light position in view space
uniform vec3 Kd;                // Diffuse reflectivity
uniform vec3 Ld;                // Light intensity
uniform mat4 ModelViewMatrix;    // Model-View matrix
uniform mat4 MVP;                // Model-View-Projection matrix
uniform mat3 NormalMatrix;       // Normal transformation matrix

uniform mat4 RotationMatrix;

void  getCamSpaceValues (out vec3 norm, out vec3 position)
{
  
  norm = normalize(NormalMatrix * VertexNormal);
  position = (ModelViewMatrix * vec4(VertexPosition,1.0)).xyz;


}

void main() 
{
    //Call getcamspacevalues
    vec3 camNorm, camPosition;
    getCamSpaceValues(camNorm, camPosition);

     Color = VertexColor;

    // Step 1: Transform vertex to clip space
    gl_Position = MVP * vec4(VertexPosition, 1.0);

    // Step 2: Convert the normal to eye coordinates & normalise
    vec3 n = normalize(NormalMatrix * VertexNormal);

    // Step 3: Convert vertex position to eye coordinates
    vec4 vertexPosEye = ModelViewMatrix * vec4(VertexPosition, 1.0);

    // Step 4: Compute the light direction (normalise the vector)
    vec3 lightDir = normalize(vec3(LightPosition) - vec3(vertexPosEye));

    // Step 5: Diffuse shading calculation using Lambert's Law
    float diff = max(dot(n, lightDir), 0.0);

    // Step 6: Compute the final light intensity
    LightIntensity = Ld * Kd * diff;
}

