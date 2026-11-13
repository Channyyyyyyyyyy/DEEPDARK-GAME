#version 460 core
in vec3 d; out vec4 o; layout(binding=6) uniform samplerCube sky; uniform vec3 mdir;
void main(){ vec3 dd = normalize(d); vec3 c = texture(sky,dd).rgb; float md = dot(dd,-mdir);
  c += smoothstep(0.9990,0.9996,md)*vec3(0.85,0.9,1.0) + pow(max(md,0.0),64.0)*vec3(0.03,0.04,0.07);
  o = vec4(pow(c, vec3(1.0/2.2)), 1.0); }
