#pragma once
// Owner: R3
#include "Common.h"
struct Mesh {
  GLuint vao = 0, vbo = 0, ebo = 0; GLsizei count = 0;
  // stride 6 = position + normal, comps = floats of position. Always drawn with glDrawElements (B2).
  void build(const std::vector<float>& v, const std::vector<unsigned>& idx, int stride, int comps);
  void draw() const;
  void destroy();
};
void addQuad(std::vector<float>& V, std::vector<unsigned>& I, glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d, glm::vec3 n);
