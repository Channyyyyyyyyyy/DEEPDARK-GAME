#pragma once
// Owner: R3 (the cube map) - drawn after the scene, at depth 1.0
#include "Common.h"
#include "Shader.h"
#include "Mesh.h"
class Sky {
public:
  bool init();
  void draw(const glm::mat4& view, const glm::mat4& proj, const glm::vec3& moonDir);
private:
  Shader sh; GLuint tex = 0; Mesh box;
};
