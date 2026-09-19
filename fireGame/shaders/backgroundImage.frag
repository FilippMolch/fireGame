#version 420 core

out vec4 FragColor;

in vec2 tex_coord;
uniform sampler2D tex;

void main() {
    vec4 result = texture(tex, vec2(tex_coord.x, 1.0f - tex_coord.y));

    result.xyz *= 0.85f;

    FragColor =  result;
}