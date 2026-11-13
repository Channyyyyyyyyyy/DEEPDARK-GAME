#version 460 core
layout(location=0) in vec3 p; uniform mat4 vp; out vec3 d;
void main(){ d = p; vec4 q = vp*vec4(p,1.0); gl_Position = q.xyww; }
