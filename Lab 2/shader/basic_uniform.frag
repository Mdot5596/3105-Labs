#version 460

flat in vec3 LightIntensity;
in vec3 Position;
in vec3 Normal;
in vec3 ViewDir;
in vec3 Color;

out vec4 FragColor;

uniform vec3 Ks;        // Specular reflectivity
uniform float Shininess; // Controls sharpness of highlights

void main() 
{
    vec3 objectColor = vec3(1.0, 0.0, 0.0); // Red colour
    FragColor = vec4(objectColor * LightIntensity, 1.0);

    // Normalize normal and view direction
    vec3 n = normalize(Normal);
    vec3 v = normalize(ViewDir);

    // Compute light direction
    vec3 lightDir = normalize(-Position); 

    // Compute reflection vector
    vec3 r = reflect(-lightDir, n);

    // Compute specular highlight (Phong model)
    float spec = pow(max(dot(r, v), 0.0), Shininess);
    vec3 specular = Ks * spec;

    // Final color = Diffuse + Specular
    vec3 finalColor = LightIntensity + specular;
}
