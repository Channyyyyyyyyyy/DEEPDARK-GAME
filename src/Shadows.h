#pragma once
// Owner: R2 (ShadowMap / OmniShadowMap equivalents)
#include "Common.h"
#include "Shader.h"
#include "Level.h"
class Shadows {
public:
  GLuint torchTex = 0, moonTex = 0, cubes[MAXL] = {0}; glm::mat4 moonVP{1.f};
  bool init();
  void renderCubes(const Level& L);                          // one layered draw pass per casting point light
  void renderMoon(const Level& L);                           // directional light, orthographic
  void renderTorch(const Level& L, const glm::mat4& vp);     // every frame: the torch moves
  void bind() const;                                         // units: 0 torch | 1..4 cubes | 5 moon  (ledger T2)
private:
  GLuint fbo = 0; Shader depth, omni;
};
