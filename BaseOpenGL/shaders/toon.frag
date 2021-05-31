#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
} fs_in;

uniform vec3 objectColor;
uniform vec3 lightPos;
uniform int numShades;
uniform int toonSetting;

void main()
{           
    float intensity;
    vec3 color = objectColor;
    // diffuse
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    vec3 normal = normalize(fs_in.Normal);
    float diff = max(dot(lightDir, normal), 0.0);

    if (toonSetting == 0)
        intensity = max(0.2, ceil(diff * numShades)/numShades);
    else if (toonSetting == 1){
        if (diff > 0.95)
		intensity = 1.0;
	else if (diff > 0.5)
		intensity = 0.6;
	else if (diff > 0.25)
		intensity = 0.4;
	else
		intensity = 0.2;}
    else {
        if (diff > 0.98){
		intensity = 1.0;
                color = vec3(1.0, 1.0, 1.0);
}
	else if (diff > 0.5)
		intensity = 0.7;
	else if (diff > 0.25)
		intensity = 0.4;
	else
		intensity = 0.2;
}

    FragColor = vec4(color * intensity, 1.0);
}