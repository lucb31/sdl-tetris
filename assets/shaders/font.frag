#version 330 core

uniform sampler2D tex;

in vec2 TexCoord;

// STATIC blue
vec3 textColor = vec3(0.0, 0.0, 1.0);

void main() {
    vec4 sampled = 1 - texture(tex, TexCoord);
    sampled.a = step(0.5, sampled.r);
    gl_FragColor = vec4(textColor, 1.0) * sampled;
}
