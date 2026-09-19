#version 420 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;

uniform mat4 viewMat;
uniform mat4 projMat;
uniform mat4 modelMat;
uniform vec3 viewPos;

out vec2 tex_coord;
out vec3 viewpos;

void main() {

    tex_coord = uv;

    gl_Position = projMat * viewMat * modelMat * vec4(position, 1.0f);

}