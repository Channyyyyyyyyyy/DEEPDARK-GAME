#include "Renderer.h"
#include <cmath>
using namespace glm;
bool Renderer::init(){
  if(!prog.load("main.vert", "main.frag")) return false;
  std::vector<float> V; std::vector<unsigned> I;   // unit cube, 24 vertices: a lit box needs one normal per face
  const vec3 N[6] = {{1,0,0},{-1,0,0},{0,1,0},{0,-1,0},{0,0,1},{0,0,-1}};
  const vec3 Uv[6] = {{0,1,0},{0,1,0},{1,0,0},{1,0,0},{1,0,0},{1,0,0}}, Vv[6] = {{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,1,0},{0,1,0}};
  for(int f = 0; f < 6; f++){
    auto P = [&](float s, float t){ return N[f] * 0.5f + Uv[f] * s * 0.5f + Vv[f] * t * 0.5f; };
    addQuad(V, I, P(-1, -1), P(1, -1), P(1, 1), P(-1, 1), N[f]);
  }
  cube.build(V, I, 6, 3); return true;
}
void Renderer::box(const vec3& p, float s, int mat, const vec3& em, bool lit){
  prog.set("lit", lit ? 1.f : 0.f); prog.set("cur", (float)mat); prog.set("off", p); prog.set("sc", s); prog.set("em", em); cube.draw();
}
void Renderer::draw(const Game& g, const Shadows& sh, const mat4& view, const mat4& proj, const mat4& torchVP, double now){
  const Level& L = g.level; const Config& c = L.cfg; const Camera& cam = g.cam;
  prog.use(); sh.bind();
  std::vector<float> lp(12, 0.f), lc(12, 0.f);
  for(size_t i = 0; i < L.lights.size(); i++) for(int k = 0; k < 3; k++){ lp[i * 3 + k] = L.lights[i].p[k]; lc[i * 3 + k] = L.lights[i].c[k]; }
  float fl = g.bat < 0.2f ? 0.75f + 0.25f * (float)(std::sin(now * 50) * std::sin(now * 13)) : 1.f;   // flicker when nearly dead
  vec3 fw = cam.forward(), md = normalize(c.mdir);
  prog.set("vp", proj * view); prog.set("tm", torchVP); prog.set("mm", sh.moonVP);
  prog.set("eye", cam.pos); prog.set("tp", cam.pos); prog.set("td", fw);
  prog.set("tatt", c.tatt); prog.set("patt", c.patt); prog.set("mdir", md); prog.set("mcol", c.mcol);
  glUniform3fv(prog.loc("mA"), 6, &c.mA[0].x); glUniform2fv(prog.loc("mS"), 6, &c.mS[0].x);
  prog.set("bat", g.bat); prog.set("sh", g.shOn ? 1.f : 0.f); prog.set("nc", (float)L.nc); prog.set("fl", fl); prog.set("amb", c.amb);
  glUniform3fv(prog.loc("lp"), 4, lp.data()); glUniform3fv(prog.loc("lc"), 4, lc.data());
  prog.set("lit", 1.f); prog.set("sc", 1.f); prog.set("off", vec3(0.f)); prog.set("em", vec3(0.f)); prog.set("cur", -1.f);
  L.mesh.draw();
  // Props are lit by the torch like everything else: a relic only shows where the beam lands.
  for(auto& l : L.lights){ box(l.p + vec3(0, 0.45f, 0), 0.5f, 5, vec3(0.f), true); box(l.p, 0.3f, 5, l.c, false); }   // brass housing + bulb
  for(auto& it : L.items){
    if(it.done) continue;
    float y = 1.1f + 0.15f * (float)std::sin(now * 2.5 + it.p.x);
    if(it.t == 'r') box({it.p.x, y, it.p.y}, 0.4f, 2, vec3(0.f), true);
    else if(it.t == 'b') box({it.p.x, y, it.p.y}, 0.3f, 3, vec3(0.f), true);
    else { box({it.p.x, 0.6f, it.p.y}, 1.2f, 4, vec3(0.f), true);
           box({it.p.x, 1.5f, it.p.y}, 0.25f, 4, g.got >= c.relics ? vec3(0.2f, 1, 0.4f) : vec3(1, 0.1f, 0.1f), false); }
  }
}
