#version 420 core

out vec4 FragColor;

in vec2 tex_coord;
in vec3 viewpos;

uniform sampler2D tex;

uniform int texID;

uniform float time;

void main() {
    vec4 result = vec4(vec3(0.0f), 0.8f);
    FragColor = result;
}