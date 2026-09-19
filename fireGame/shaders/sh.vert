#version 420 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 norm;
layout (location = 2) in vec2 uv;

uniform mat4 viewMat;
uniform mat4 projMat;
uniform mat4 modelMat;
uniform vec3 viewPos;

out mat4 finMat;
out vec3 normal;
out vec3 FragPos;
out vec2 tex_coord;
out vec3 viewpos;

const float dens = 0.007f;
const float grad = 1.5f;

void main() {
    finMat = viewMat;
    mat4 fin = projMat * viewMat * modelMat;

    tex_coord = uv;

    viewpos = viewPos;

    normal = mat3(transpose(inverse(modelMat))) * norm;
    FragPos = vec3(modelMat * vec4(position, 1.0f));
    gl_Position = fin * vec4(position, 1.0f);
}