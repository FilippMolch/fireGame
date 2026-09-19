#version 420 core
out vec4 FragColor;

in vec2 tex_coord;

uniform sampler2D tex;

void main() {
    FragColor = texture(tex, tex_coord);
    FragColor.rgb = pow(FragColor.rgb, vec3(1.0f/2.2f));
    FragColor.rgb = ((FragColor.rgb - 0.5f) * max(1.35f, 0)) + 0.5f;
    FragColor.rgb += 0.1f;
}