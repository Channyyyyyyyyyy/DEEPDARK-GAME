#version 460 core
in vec3 wp; in vec3 n; out vec4 o;
uniform vec3 eye, tp, td, em, lp[4], lc[4], tatt, patt, mdir, mcol;
uniform vec3 mA[6]; uniform vec2 mS[6];
uniform mat4 tm, mm;
uniform float bat, sh, nc, lit, fl, amb, cur;
layout(binding=0) uniform sampler2DShadow ts;
layout(binding=1) uniform samplerCubeShadow c0;
layout(binding=2) uniform samplerCubeShadow c1;
layout(binding=3) uniform samplerCubeShadow c2;
layout(binding=4) uniform samplerCubeShadow c3;
layout(binding=5) uniform sampler2DShadow ms;
const float FARP = 40.0, NEARP = 0.1;
float cs(int i, vec3 d, float r){ vec4 q = vec4(d,r);
  return i==0 ? texture(c0,q) : (i==1 ? texture(c1,q) : (i==2 ? texture(c2,q) : texture(c3,q))); }
void main(){
  if(lit < 0.5){ o = vec4(em,1.0); return; }
  vec3 N = normalize(n), V = normalize(eye - wp);
  vec3 alb; float spI, shn;
  if(cur < -0.5){                                   // the maze itself: brick walls, stone floor
    if(abs(N.y) > 0.5){
      alb = mA[1]*(0.75+0.25*mod(floor(wp.x/2.0)+floor(wp.z/2.0),2.0)); spI = mS[1].x; shn = mS[1].y;
    } else {
      vec2 b = vec2((wp.x+wp.z)*0.7, wp.y*2.5); float row = floor(b.y); b.x += row*0.5; vec2 f = fract(b);
      float mo = min(step(f.x,0.06)+step(f.y,0.12), 1.0);
      vec3 br = mA[0]*(0.7+0.3*fract(sin(floor(b.x)*12.9+row*78.2)*437.5));
      alb = mix(br, vec3(0.18), mo); spI = mS[0].x*(1.0-mo); shn = mS[0].y;
    }
  } else { int m = int(cur+0.5); alb = mA[m]; spI = mS[m].x; shn = mS[m].y; }
  vec3 c = alb*amb;
  // ---- torch: spot light, battery-scaled attenuation (E3), one 40-degree shadow map (E2)
  vec3 L = tp - wp; float d = length(L); L /= d;
  float sp = smoothstep(0.94,0.975,dot(-L,td)) * step(0.001,bat);
  float s = mix(0.25,1.0,bat);                           // radius scale: d -> d/s
  float at = tatt.x + tatt.y*d/s + tatt.z*d*d/(s*s);    // constant untouched, linear/s, quadratic/s^2
  vec4 q = tm*vec4(wp+N*0.04,1.0); vec3 pc = q.xyz/q.w*0.5+0.5;
  float vis = sh > 0.5 ? texture(ts, vec3(pc.xy, pc.z-0.0015)) : 1.0;
  c += (alb*max(dot(N,L),0.0) + pow(max(dot(N,normalize(L+V)),0.0),shn)*spI) * vec3(1.0,0.93,0.82)*sp*vis*3.0*fl/at;
  // ---- fixed point lights; the first nc cast omnidirectional shadows
  for(int i=0;i<4;i++){
    vec3 Lp = lp[i]-wp; float dd = length(Lp); Lp /= dd;
    float a = patt.x + patt.y*dd + patt.z*dd*dd, v = 1.0;
    if(sh > 0.5 && float(i) < nc){
      vec3 fd = wp + N*0.06 - lp[i];
      float m = max(abs(fd.x), max(abs(fd.y), abs(fd.z)));
      float z = (FARP+NEARP)/(FARP-NEARP) - 2.0*FARP*NEARP/((FARP-NEARP)*m);
      v = cs(i, fd, z*0.5+0.5-0.0008);
    }
    c += (alb*max(dot(N,Lp),0.0) + pow(max(dot(N,normalize(Lp+V)),0.0),shn)*spI*0.5) * lc[i]*v*1.6/a;
  }
  // ---- the moon: one directional light, orthographic shadow map, very dim
  vec4 mq = mm*vec4(wp+N*0.08,1.0); vec3 mc = mq.xyz*0.5+0.5;
  float mv = sh > 0.5 ? texture(ms, vec3(mc.xy, mc.z-0.0008)) : 1.0;
  c += alb*max(dot(N,-mdir),0.0)*mcol*mv;
  o = vec4(pow(c, vec3(1.0/2.2)), 1.0);
}
