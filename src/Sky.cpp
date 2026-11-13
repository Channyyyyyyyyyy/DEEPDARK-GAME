#include "Sky.h"
#include <algorithm>
#include <random>
using namespace glm;
// B10: a procedural night-sky cube map. Six 256x256 faces of dark blue with random stars, so no image file
// and no face-orientation mistakes are possible. Faces are uploaded in the order +X -X +Y -Y +Z -Z.
bool Sky::init(){
  if(!sh.load("sky.vert", "sky.frag")) return false;
  glGenTextures(1, &tex); glBindTexture(GL_TEXTURE_CUBE_MAP, tex); std::mt19937 r(12345);
  for(int f = 0; f < 6; f++){
    std::vector<unsigned char> px(256 * 256 * 3);
    for(int i = 0; i < 256 * 256; i++){ px[i * 3] = 5; px[i * 3 + 1] = 8; px[i * 3 + 2] = 22; }
    for(int s = 0; s < 220; s++){
      int x = r() % 256, y = r() % 256, b = 90 + (int)(r() % 166), sz = (r() % 12 == 0) ? 2 : 1;
      for(int dy = 0; dy < sz; dy++) for(int dx = 0; dx < sz; dx++) if(x + dx < 256 && y + dy < 256){
        unsigned char* p = &px[((y + dy) * 256 + x + dx) * 3]; p[0] = (unsigned char)b; p[1] = (unsigned char)b; p[2] = (unsigned char)std::min(255, b + 20); }
    }
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + f, 0, GL_RGB8, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, px.data());
  }
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR); glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE); glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
  // The sky box really does use 8 corners: the sky shader uses the position as the direction and ignores normals.
  std::vector<float> V = {-1,-1,-1, 1,-1,-1, 1,1,-1, -1,1,-1, -1,-1,1, 1,-1,1, 1,1,1, -1,1,1};
  std::vector<unsigned> I = {0,1,2,0,2,3, 4,6,5,4,7,6, 0,4,5,0,5,1, 3,2,6,3,6,7, 0,3,7,0,7,4, 1,5,6,1,6,2};
  box.build(V, I, 3, 3); return true;
}
void Sky::draw(const mat4& view, const mat4& proj, const vec3& moonDir){
  glDepthFunc(GL_LEQUAL); sh.use(); glBindTextureUnit(6, tex);     // unit 6: only this program reads it
  sh.set("vp", proj * mat4(mat3(view))); sh.set("mdir", moonDir);  // mat3 cast drops the translation: the sky follows the eye
  box.draw(); glDepthFunc(GL_LESS);
}
