#version 330

uniform mat4 mvp;
layout (location = 0) in vec2 pos;
layout (location = 1) in vec2 uv;

out vec2 TexCoord;

void main() {
    gl_Position = mvp * vec4(pos, 0.0, 1.0);
    TexCoord = uv;
}
