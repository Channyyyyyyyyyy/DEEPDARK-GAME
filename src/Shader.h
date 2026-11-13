#pragma once
// Owner: R2
#include "Common.h"
#include <unordered_map>
class Shader {
public:
  GLuint id = 0;
  bool load(const std::string& vert, const std::string& frag, const std::string& geom = "");  // names inside Shaders/
  void use() const { glUseProgram(id); }
  GLint loc(const char* n) const;
  void set(const char* n, float v) const { glUniform1f(loc(n), v); }
  void set(const char* n, const glm::vec2& v) const { glUniform2fv(loc(n), 1, glm::value_ptr(v)); }
  void set(const char* n, const glm::vec3& v) const { glUniform3fv(loc(n), 1, glm::value_ptr(v)); }
  void set(const char* n, const glm::vec4& v) const { glUniform4fv(loc(n), 1, glm::value_ptr(v)); }
  void set(const char* n, const glm::mat4& v) const { glUniformMatrix4fv(loc(n), 1, GL_FALSE, glm::value_ptr(v)); }
private:
  mutable std::unordered_map<std::string, GLint> cache;
};
