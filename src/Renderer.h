#pragma once
// Owner: R2 - the main scene pass.
#include "Common.h"
#include "Shader.h"
#include "Mesh.h"
#include "Game.h"
#include "Shadows.h"
class Renderer {
public:
  bool init();
  void draw(const Game& g, const Shadows& sh, const glm::mat4& view, const glm::mat4& proj, const glm::mat4& torchVP, double now);
private:
  Shader prog; Mesh cube;
  void box(const glm::vec3& p, float s, int mat, const glm::vec3& em, bool lit);
};
