#version 420 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;

uniform mat4 viewMat;
uniform mat4 projMat;
uniform mat4 modelMat;

out vec3 FragCoord;

void main() {
    gl_Position = projMat * viewMat * modelMat * vec4(position, 1.0f);
    FragCoord = position;
}