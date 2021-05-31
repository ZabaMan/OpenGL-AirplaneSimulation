#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 Position;

uniform vec3 cameraPos;
uniform samplerCube skybox;

uniform float EtaR;
uniform float EtaG;
uniform float EtaB;
uniform float FresnelPower;

void main()
{   
    float ratio = 1.00 / 1.52;
    vec3 I = normalize(Position - cameraPos);

    float F = ((1.0 - EtaG) * (1.0-EtaG)) / ((1.0+EtaG) * (1.0+EtaG));
    float Ratio = F + (1.0 - F) * pow((1.0 - dot(-I, normalize(Normal))), FresnelPower);

    vec3 RefractR = refract(I, normalize(Normal), EtaR);
    vec3 RefractG = refract(I, normalize(Normal), EtaG);
    vec3 RefractB = refract(I, normalize(Normal), EtaB);
    vec3 Reflect = reflect(I, normalize(Normal));

    vec3 Refract;
    Refract.r = (texture(skybox, RefractR).rgb).r;
    Refract.g = (texture(skybox, RefractG).rgb).g;
    Refract.b = (texture(skybox, RefractB).rgb).b;

    Reflect = texture(skybox, Reflect).rgb;

    vec3 RefractReflect = mix(Reflect, Refract, Ratio);

    FragColor = vec4(RefractReflect, 1.0);
}  