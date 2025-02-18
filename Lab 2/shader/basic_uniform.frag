#version 460

in vec3 LightIntensity;         // Received from vertex shader
in vec3 Color;

layout(location = 0) out vec4 FragColor;  // Final color output


void main()
{
    // Use LightIntensity as the final fragment color
    FragColor = vec4(LightIntensity, 1.0);
    FragColor = vec4(Color, 1.0);
}
