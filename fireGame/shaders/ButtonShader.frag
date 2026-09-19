#version 420 core

out vec4 FragColor;

in vec2 tex_coord;
in vec3 viewpos;

uniform float time;

uniform int focused;

void main() {

    vec4 result = vec4(vec3(0.1f), 1.0f);

    if (focused == 1)
        result = vec4(vec3(0.3f), 1.0f);

    FragColor = result;
}