#include "Hud.h"
using namespace glm;
// A-Z, 0-9, space  /  %  :  .  -   (5 bits per row, top row first)
static const unsigned char FONT[42][7] = {
 {14,17,17,31,17,17,17},{30,17,17,30,17,17,30},{14,17,16,16,16,17,14},{30,17,17,17,17,17,30},{31,16,16,30,16,16,31},
 {31,16,16,30,16,16,16},{14,17,16,23,17,17,15},{17,17,17,31,17,17,17},{14,4,4,4,4,4,14},{7,2,2,2,2,18,12},
 {17,18,20,24,20,18,17},{16,16,16,16,16,16,31},{17,27,21,21,17,17,17},{17,25,21,19,17,17,17},{14,17,17,17,17,17,14},
 {30,17,17,30,16,16,16},{14,17,17,17,21,18,13},{30,17,17,30,20,18,17},{15,16,16,14,1,1,30},{31,4,4,4,4,4,4},
 {17,17,17,17,17,17,14},{17,17,17,17,17,10,4},{17,17,17,21,21,27,17},{17,17,10,4,10,17,17},{17,17,10,4,4,4,4},
 {31,1,2,4,8,16,31},
 {14,17,19,21,25,17,14},{4,12,4,4,4,4,14},{14,17,1,2,4,8,31},{30,1,1,14,1,1,30},{2,6,10,18,31,2,2},
 {31,16,30,1,1,17,14},{6,8,16,30,17,17,14},{31,1,2,4,8,8,8},{14,17,17,14,17,17,14},{14,17,17,15,1,2,12},
 {0,0,0,0,0,0,0},{1,2,2,4,8,8,16},{25,26,2,4,11,19,3},{0,4,4,0,4,4,0},{0,0,0,0,0,12,12},{0,0,0,31,0,0,0}};
static int glyph(char c){
  if(c >= 'a' && c <= 'z') c -= 32;
  if(c >= 'A' && c <= 'Z') return c - 'A'; if(c >= '0' && c <= '9') return 26 + c - '0';
  switch(c){ case '/': return 37; case '%': return 38; case ':': return 39; case '.': return 40; case '-': return 41; }
  return 36;
}
bool Hud::init(){
  if(!sh.load("hud.vert", "hud.frag")) return false;
  lPos = sh.loc("pos"); lSize = sh.loc("size"); lCol = sh.loc("col");
  std::vector<float> Q = {0,0, 1,0, 1,1, 0,1}; std::vector<unsigned> QI = {0,1,2, 0,2,3}; quad.build(Q, QI, 2, 2); return true;
}
void Hud::begin(int w, int h){
  width = w; height = h; glDisable(GL_DEPTH_TEST); glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); sh.use();
  // Orthographic, because a HUD has no depth and must not shrink with distance. (The directional light uses ortho for the same reason.)
  sh.set("proj", ortho(0.f, (float)w, 0.f, (float)h, -1.f, 1.f));
}
void Hud::end(){ glDisable(GL_BLEND); glEnable(GL_DEPTH_TEST); }
void Hud::rect(float x, float y, float w, float h, const vec4& c){
  glUniform2f(lPos, x, y); glUniform2f(lSize, w, h); glUniform4fv(lCol, 1, value_ptr(c)); quad.draw();
}
float Hud::textW(const std::string& s, float sc) const { return s.empty() ? 0.f : (6.f * s.size() - 1.f) * sc; }
void Hud::text(const std::string& s, float x, float y, float sc, const vec4& c){
  for(char ch : s){
    int g = glyph(ch);
    for(int r = 0; r < 7; r++) for(int k = 0; k < 5; k++) if(FONT[g][r] & (16 >> k)) rect(x + k * sc, y + (6 - r) * sc, sc, sc, c);
    x += 6 * sc;
  }
}
void Hud::textC(const std::string& s, float y, float sc, const vec4& c){ text(s, (width - textW(s, sc)) * 0.5f, y, sc, c); }
