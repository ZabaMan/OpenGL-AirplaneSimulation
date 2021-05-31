#version 330 core
out vec4 FragColor;
 
struct Material {
    vec3 ambient;
    vec3 diffuse;
}; 

struct Light {
    vec3 position;
};

uniform Material material;
uniform Light light;  
 
uniform vec3 objectColor;
uniform vec3 viewPos;

uniform float darkenCoefficient;

in VS_OUT {
   in vec3 FragPos;
   in vec3 Normal;
} fs_in;


void main()
{
    // ambient
    vec3 ambient = material.ambient * objectColor;

    // light direction and normal
    vec3 norm = normalize(fs_in.Normal);
    vec3 lightDir = light.position - fs_in.FragPos;
    float distance = length(lightDir);
    lightDir = normalize(lightDir);
    float lambertian = max(dot(lightDir, norm), 0.0);
    vec3 diffuse = lambertian * objectColor;

    vec3 viewDir = normalize(viewPos - fs_in.FragPos);

    // minnaert
    float nDotL = dot(norm, lightDir);
    float nDotE = dot(norm, viewDir);

    float intensity = max(clamp(nDotL, 0.0, 1.0) * pow(nDotL * nDotE, darkenCoefficient) / distance, 0);

    vec3 result = ambient + intensity * diffuse;

    FragColor = vec4(result, 1.0);
}