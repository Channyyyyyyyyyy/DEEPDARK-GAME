#pragma once
// Owner: R4 - the on-screen drawing primitives (B12). What gets drawn lives in Game::drawHud.
#include "Common.h"
#include "Shader.h"
#include "Mesh.h"
class Hud {
public:
  int width = 1280, height = 720;
  bool init();
  void begin(int w, int h);        // flat overlay: glm::ortho, 1 unit = 1 pixel, origin bottom-left, no depth test
  void end();
  void rect(float x, float y, float w, float h, const glm::vec4& c);
  void text(const std::string& s, float x, float y, float scale, const glm::vec4& c);   // 5x7 bitmap font, no text library
  void textC(const std::string& s, float y, float scale, const glm::vec4& c);           // centred
  float textW(const std::string& s, float scale) const;
private:
  Shader sh; Mesh quad; GLint lPos = -1, lSize = -1, lCol = -1;
};
