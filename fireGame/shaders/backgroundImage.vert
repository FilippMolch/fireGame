#version 420 core
layout (location = 0) in vec2 position;
layout (location = 1) in vec2 uv;

uniform mat4 projMat;
uniform mat4 modelMat;

out vec2 tex_coord;
out vec3 viewpos;

void main() {

    tex_coord = uv;
    gl_Position = modelMat * vec4(position, 0.0f, 1.0f);

}