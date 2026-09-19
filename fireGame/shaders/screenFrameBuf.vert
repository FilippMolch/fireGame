#version 420 core
layout (location = 0) in vec2 position;
layout (location = 1) in vec2 TexCoord;

out vec2 tex_coord;

uniform mat4 screenRatioCorrect;

void main() {
    tex_coord = TexCoord;
    gl_Position = screenRatioCorrect * vec4(position, 0.0f, 1.0f);
}