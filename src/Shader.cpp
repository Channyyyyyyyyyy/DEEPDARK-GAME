#include "Shader.h"
#include <cstdio>
#include <fstream>
#include <sstream>

bool findAndRead(const std::string& rel, std::string& out){
  for(const char* prefix : {"", "../", "../../", "../../../"}){
    std::ifstream f(std::string(prefix) + rel);
    if(f){ std::stringstream ss; ss << f.rdbuf(); out = ss.str(); return true; }
  }
  return false;
}
static GLuint compile(GLenum type, const std::string& name){
  std::string src;
  if(!findAndRead("Shaders/" + name, src)){ fprintf(stderr, "Shader file not found: Shaders/%s\n", name.c_str()); return 0; }
  GLuint h = glCreateShader(type); const char* c = src.c_str(); glShaderSource(h, 1, &c, nullptr); glCompileShader(h);
  GLint ok; glGetShaderiv(h, GL_COMPILE_STATUS, &ok);
  if(!ok){ char log[2048]; glGetShaderInfoLog(h, 2048, nullptr, log); fprintf(stderr, "Shader error in %s:\n%s\n", name.c_str(), log); }
  return h;
}
bool Shader::load(const std::string& vert, const std::string& frag, const std::string& geom){
  GLuint v = compile(GL_VERTEX_SHADER, vert), f = compile(GL_FRAGMENT_SHADER, frag), g = geom.empty() ? 0 : compile(GL_GEOMETRY_SHADER, geom);
  if(!v || !f || (!geom.empty() && !g)) return false;
  id = glCreateProgram(); glAttachShader(id, v); glAttachShader(id, f); if(g) glAttachShader(id, g);
  glLinkProgram(id); GLint ok; glGetProgramiv(id, GL_LINK_STATUS, &ok);
  if(!ok){ char log[2048]; glGetProgramInfoLog(id, 2048, nullptr, log); fprintf(stderr, "Link error (%s + %s):\n%s\n", vert.c_str(), frag.c_str(), log); return false; }
  glDeleteShader(v); glDeleteShader(f); if(g) glDeleteShader(g);
  return true;
}
GLint Shader::loc(const char* n) const {
  auto it = cache.find(n); if(it != cache.end()) return it->second;
  GLint l = glGetUniformLocation(id, n); cache[n] = l; return l;
}
