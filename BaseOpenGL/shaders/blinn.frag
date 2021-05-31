#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

struct Light {
    vec3 position;
};

uniform vec3 objectColor;
uniform Light light; 
uniform vec3 viewPos;

uniform float specSize;
uniform float specStrength;
uniform float ambient;


void main()
{           
    vec3 color = objectColor;
    // ambient
    vec3 ambient = ambient * color;
    // diffuse
    vec3 lightDir = normalize(light.position - fs_in.FragPos);
    vec3 normal = normalize(fs_in.Normal);
    float lambertian = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = lambertian * color;
    // specular
    vec3 viewDir = normalize(- fs_in.FragPos);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(halfwayDir, normal), 0.0), specStrength);
    vec3 specular = vec3(specSize) * spec; // assuming bright white light color
    FragColor = vec4(ambient + diffuse + specular, 1.0);
}