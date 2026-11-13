#version 460 core
layout(location=0) in vec3 p; layout(location=1) in vec3 nn;
uniform mat4 vp; uniform vec3 off; uniform float sc;
out vec3 wp; out vec3 n;
void main(){ wp = p*sc + off; n = nn; gl_Position = vp*vec4(wp,1.0); }
