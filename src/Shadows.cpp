#include "Shadows.h"
#include <algorithm>
#include <cmath>
using namespace glm;
static void depthParams(GLenum t){
  glTexParameteri(t, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE); glTexParameteri(t, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
  glTexParameteri(t, GL_TEXTURE_MIN_FILTER, GL_LINEAR); glTexParameteri(t, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(t, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); glTexParameteri(t, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}
bool Shadows::init(){
  if(!depth.load("main.vert", "depth.frag") || !omni.load("omni.vert", "depth.frag", "omni.geom")) return false;
  glGenFramebuffers(1, &fbo); glBindFramebuffer(GL_FRAMEBUFFER, fbo); glDrawBuffer(GL_NONE); glReadBuffer(GL_NONE);
  glGenTextures(1, &torchTex); glBindTexture(GL_TEXTURE_2D, torchTex); glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT24, TS, TS); depthParams(GL_TEXTURE_2D);
  glGenTextures(1, &moonTex); glBindTexture(GL_TEXTURE_2D, moonTex); glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT24, MAPMOON, MAPMOON); depthParams(GL_TEXTURE_2D);
  for(int i = 0; i < MAXL; i++){
    glGenTextures(1, &cubes[i]); glBindTexture(GL_TEXTURE_CUBE_MAP, cubes[i]);
    glTexStorage2D(GL_TEXTURE_CUBE_MAP, 1, GL_DEPTH_COMPONENT24, CS, CS); depthParams(GL_TEXTURE_CUBE_MAP);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  }
  return true;
}
void Shadows::bind() const {
  glBindTextureUnit(0, torchTex); for(int i = 0; i < MAXL; i++) glBindTextureUnit(1 + i, cubes[i]); glBindTextureUnit(5, moonTex);
}
// One glFramebufferTexture attaches the WHOLE cube map, so gl_Layer works: ONE draw call per light, and the
// geometry shader fans each triangle out to six faces. 1 draw pass, 6x the rasterised triangles.
void Shadows::renderCubes(const Level& L){
  glBindFramebuffer(GL_FRAMEBUFFER, fbo); glViewport(0, 0, CS, CS); omni.use();
  glEnable(GL_DEPTH_TEST); glDepthMask(GL_TRUE); glEnable(GL_POLYGON_OFFSET_FILL); glPolygonOffset(2, 4);
  const vec3 dirs[6] = {{1, 0, 0}, {-1, 0, 0}, {0, 1, 0}, {0, -1, 0}, {0, 0, 1}, {0, 0, -1}};
  const vec3 ups[6]  = {{0, -1, 0}, {0, -1, 0}, {0, 0, 1}, {0, 0, -1}, {0, -1, 0}, {0, -1, 0}};
  for(int i = 0; i < L.nc; i++){
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, cubes[i], 0); glClear(GL_DEPTH_BUFFER_BIT);
    mat4 pr = perspective(radians(90.f), 1.f, 0.1f, FARP), f[6];
    for(int k = 0; k < 6; k++) f[k] = pr * lookAt(L.lights[i].p, L.lights[i].p + dirs[k], ups[k]);
    glUniformMatrix4fv(omni.loc("faces"), 6, GL_FALSE, value_ptr(f[0])); L.mesh.draw();
  }
  glDisable(GL_POLYGON_OFFSET_FILL);
}
void Shadows::renderMoon(const Level& L){
  vec3 d = normalize(L.cfg.mdir), ctr(L.GW * S * 0.5f, 0.f, L.GH * S * 0.5f); float R = 0.5f * std::hypot(L.GW * S, L.GH * S) + 6.f;
  vec3 up = std::fabs(d.y) > 0.99f ? vec3(0, 0, 1) : vec3(0, 1, 0);
  moonVP = ortho(-R, R, -R, R, 0.1f, R * 2.6f) * lookAt(ctr - d * R * 1.2f, ctr, up);   // directional light: parallel rays, so ortho
  glBindFramebuffer(GL_FRAMEBUFFER, fbo); glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, moonTex, 0);
  glViewport(0, 0, MAPMOON, MAPMOON); glEnable(GL_DEPTH_TEST); glDepthMask(GL_TRUE); glClear(GL_DEPTH_BUFFER_BIT);
  glEnable(GL_POLYGON_OFFSET_FILL); glPolygonOffset(2, 4); depth.use();
  depth.set("vp", moonVP); depth.set("sc", 1.f); depth.set("off", vec3(0.f)); L.mesh.draw(); glDisable(GL_POLYGON_OFFSET_FILL);
}
void Shadows::renderTorch(const Level& L, const mat4& vp){
  glBindFramebuffer(GL_FRAMEBUFFER, fbo); glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, torchTex, 0);
  glViewport(0, 0, TS, TS); glEnable(GL_DEPTH_TEST); glDepthMask(GL_TRUE); glClear(GL_DEPTH_BUFFER_BIT);
  glEnable(GL_POLYGON_OFFSET_FILL); glPolygonOffset(2, 4); depth.use();
  depth.set("vp", vp); depth.set("sc", 1.f); depth.set("off", vec3(0.f)); L.mesh.draw(); glDisable(GL_POLYGON_OFFSET_FILL);
}
