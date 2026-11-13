#version 460 core
layout(location=0) in vec2 p; uniform mat4 proj; uniform vec2 pos, size;
void main(){ gl_Position = proj*vec4(pos + p*size, 0.0, 1.0); }
