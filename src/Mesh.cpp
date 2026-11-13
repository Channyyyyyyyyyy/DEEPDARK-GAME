#include "Mesh.h"
void Mesh::build(const std::vector<float>& v, const std::vector<unsigned>& idx, int stride, int comps){
  destroy(); count = (GLsizei)idx.size();
  glGenVertexArrays(1, &vao); glBindVertexArray(vao);
  glGenBuffers(1, &vbo); glBindBuffer(GL_ARRAY_BUFFER, vbo); glBufferData(GL_ARRAY_BUFFER, v.size() * 4, v.data(), GL_STATIC_DRAW);
  glGenBuffers(1, &ebo); glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo); glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx.size() * 4, idx.data(), GL_STATIC_DRAW);
  glEnableVertexAttribArray(0); glVertexAttribPointer(0, comps, GL_FLOAT, GL_FALSE, stride * 4, nullptr);
  if(stride == 6){ glEnableVertexAttribArray(1); glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride * 4, (void*)12); }
}
void Mesh::draw() const { glBindVertexArray(vao); glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr); }
void Mesh::destroy(){ if(vao){ glDeleteVertexArrays(1, &vao); glDeleteBuffers(1, &vbo); glDeleteBuffers(1, &ebo); vao = 0; } }
// 4 vertices with the face normal + 6 indices. A lit box needs 24 vertices, not 8: one normal per vertex.
void addQuad(std::vector<float>& V, std::vector<unsigned>& I, glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d, glm::vec3 n){
  unsigned base = (unsigned)(V.size() / 6);
  for(glm::vec3 p : {a, b, c, d}) V.insert(V.end(), {p.x, p.y, p.z, n.x, n.y, n.z});
  I.insert(I.end(), {base, base + 1, base + 2, base, base + 2, base + 3});
}
