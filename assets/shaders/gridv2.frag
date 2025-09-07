#version 330 core

// WHITE
vec4 grid_color = vec4(1.0);

void main()
{
    float xHit = step(mod(gl_FragCoord.x, 32), 1);
    float yHit = step(mod(gl_FragCoord.y, 32), 1);
    float mult = xHit + yHit;
    gl_FragColor = mult * grid_color;
}
