#version 420 core

out vec4 FragColor;

in mat4 finMat;
in vec3 normal;
in vec3 FragPos;
in vec2 tex_coord;
in vec3 viewpos;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 objectColor;
uniform sampler2D tex;

void main() {
    float ambientStrength = 0.1f;
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Specular
    float specularStrength = 0.5f;
    vec3 viewDir = normalize(viewpos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    float dist = length(viewpos - FragPos);
    float v = clamp(1.0f - 1 / exp((dist*dist) * 0.0003 ), 0.0f, 1.0f);

    vec3 fogColor = vec3(0.3f);
    vec3 light = ((ambient + diffuse + specular) * objectColor);
    vec3 result = mix(texture(tex, tex_coord).xyz * light, fogColor, v);

    FragColor =  vec4(result, 1.0f);
}