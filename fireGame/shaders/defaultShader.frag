#version 420 core

out vec4 FragColor;

in vec2 tex_coord;
in vec3 viewpos;

uniform sampler2D tex;

uniform int texID;

uniform float time;

void main() {
    vec4 result = texture(tex, tex_coord);

    if (texID == 0) {
        result = vec4(vec3(0.0f), 1.0f);
    }

    FragColor =  result;
}