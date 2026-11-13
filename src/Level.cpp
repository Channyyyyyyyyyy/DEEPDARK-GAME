#include "Level.h"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <queue>
#include <random>
#include <sstream>
using namespace glm;

static const char* MATNAMES[6] = {"brick", "stone", "gold", "battery", "steel", "brass"};
static const char* DEFAULT_LEVEL = "size 12 9\nseed 7\nrelics 3\nbatteries 5\nbattery 100\ndrain 0.55\nbudget 2\nlight 2 2 1 0.45 0.25\nlight 8 4 0.3 0.5 1\nlight 4 7 0.4 1 0.4\nlight 10 1 0.9 0.3 0.6\n";

Config::Config(){
  mA[0] = {0.50f, 0.26f, 0.20f}; mS[0] = {0.10f, 8.f};     // brick
  mA[1] = {0.28f, 0.27f, 0.25f}; mS[1] = {0.50f, 40.f};    // wet stone floor
  mA[2] = {1.00f, 0.78f, 0.25f}; mS[2] = {1.20f, 96.f};    // gold relic
  mA[3] = {0.15f, 0.85f, 0.25f}; mS[3] = {0.50f, 16.f};    // green plastic battery
  mA[4] = {0.65f, 0.68f, 0.72f}; mS[4] = {1.50f, 160.f};   // steel exit
  mA[5] = {0.75f, 0.55f, 0.25f}; mS[5] = {0.90f, 48.f};    // brass lamp housing
}
Config parseLevel(const std::string& txt){
  Config c; std::istringstream in(txt); std::string line; int ln = 0;
  while(std::getline(in, line)){
    ln++; std::istringstream l(line); std::string k; l >> k; if(k.empty() || k[0] == '#') continue;
    if(k == "size") l >> c.cw >> c.ch; else if(k == "seed") l >> c.seed; else if(k == "relics") l >> c.relics;
    else if(k == "batteries") l >> c.batteries; else if(k == "battery") l >> c.battery; else if(k == "drain") l >> c.drain;
    else if(k == "budget") l >> c.budget; else if(k == "ambient") l >> c.amb;
    else if(k == "torch") l >> c.tatt.x >> c.tatt.y >> c.tatt.z; else if(k == "pointatt") l >> c.patt.x >> c.patt.y >> c.patt.z;
    else if(k == "moon") l >> c.mdir.x >> c.mdir.y >> c.mdir.z >> c.mcol.x >> c.mcol.y >> c.mcol.z;
    else if(k == "material"){
      std::string nm; float r, g, b, sp, sh;
      if(l >> nm >> r >> g >> b >> sp >> sh){
        bool ok = false; for(int i = 0; i < 6; i++) if(nm == MATNAMES[i]){ c.mA[i] = {r, g, b}; c.mS[i] = {sp, sh}; ok = true; }
        if(!ok) fprintf(stderr, "level.txt line %d: unknown material '%s'\n", ln, nm.c_str());
      } else fprintf(stderr, "level.txt line %d: material needs: name r g b specular shininess\n", ln);
    } else if(k == "light"){
      float x, y, r, g, b;
      if(l >> x >> y >> r >> g >> b){ c.lightCells.push_back({x, y}); c.lights.push_back({r, g, b}); }
      else fprintf(stderr, "level.txt line %d: light needs: cellX cellY r g b\n", ln);
    } else fprintf(stderr, "level.txt line %d: unknown key '%s'\n", ln, k.c_str());
  }
  c.cw = std::max(c.cw, 3); c.ch = std::max(c.ch, 3); return c;
}

bool Level::solidAt(float x, float z) const {
  int gx = (int)std::floor(x / S), gz = (int)std::floor(z / S);
  return gx < 0 || gz < 0 || gx >= GW || gz >= GH || G[gz][gx] != 0;
}
bool Level::hit(float x, float z) const {
  const float r = 0.35f; return solidAt(x - r, z - r) || solidAt(x + r, z - r) || solidAt(x - r, z + r) || solidAt(x + r, z + r);
}

void Level::build(int levelNo){
  std::string text; if(!findAndRead("level.txt", text)){ printf("level.txt not found, using built-in default\n"); text = DEFAULT_LEVEL; }
  cfg = parseLevel(text); int baseCw = cfg.cw, L = levelNo - 1;      // difficulty grows with the level
  cfg.cw = std::min(cfg.cw + 2 * L, 30); cfg.ch = std::min(cfg.ch + L, 22); cfg.relics += L; cfg.drain *= 1.f + 0.15f * L;
  cfg.batteries = std::max(2, cfg.batteries - L / 2); cfg.seed += L;
  GW = 2 * cfg.cw + 1; GH = 2 * cfg.ch + 1;
  std::mt19937 rng(cfg.seed); auto rnd = [&](int n){ return (int)(rng() % n); };
  G.assign(GH, std::vector<int>(GW, 1));
  std::vector<std::vector<char>> vis(cfg.ch, std::vector<char>(cfg.cw, 0));
  std::vector<std::pair<int, int>> stk{{0, 0}}; vis[0][0] = 1; G[1][1] = 0;
  const int dx4[4] = {1, -1, 0, 0}, dy4[4] = {0, 0, 1, -1};
  while(!stk.empty()){                                              // depth-first maze carving
    auto [x, y] = stk.back(); std::vector<int> nb;
    for(int k = 0; k < 4; k++){ int a = x + dx4[k], b = y + dy4[k]; if(a >= 0 && b >= 0 && a < cfg.cw && b < cfg.ch && !vis[b][a]) nb.push_back(k); }
    if(nb.empty()){ stk.pop_back(); continue; }
    int k = nb[rnd((int)nb.size())]; int a = x + dx4[k], b = y + dy4[k]; vis[b][a] = 1;
    G[2 * y + 1 + dy4[k]][2 * x + 1 + dx4[k]] = 0; G[2 * b + 1][2 * a + 1] = 0; stk.push_back({a, b});
  }
  for(int i = 0; i < cfg.cw * cfg.ch / 4; i++){ int x = 1 + rnd(GW - 2), y = 1 + rnd(GH - 2); if((x + y) % 2 == 1) G[y][x] = 0; }   // loops
  std::vector<std::vector<int>> D(GH, std::vector<int>(GW, -1)); D[1][1] = 0; std::queue<std::pair<int, int>> q; q.push({1, 1});
  std::vector<std::pair<int, int>> cells;
  while(!q.empty()){
    auto [x, y] = q.front(); q.pop(); if(x % 2 && y % 2) cells.push_back({x, y});
    for(int k = 0; k < 4; k++){ int a = x + dx4[k], b = y + dy4[k]; if(a >= 0 && b >= 0 && a < GW && b < GH && G[b][a] == 0 && D[b][a] < 0){ D[b][a] = D[y][x] + 1; q.push({a, b}); } }
  }
  std::sort(cells.begin(), cells.end(), [&](auto& a, auto& b){ return D[a.second][a.first] > D[b.second][b.first]; });
  int far = D[cells[0].second][cells[0].first];
  auto cp = [&](std::pair<int, int> c){ return vec2((c.first + 0.5f) * S, (c.second + 0.5f) * S); };
  items.clear(); items.push_back({'e', cp(cells[0])});
  std::vector<std::pair<int, int>> pool, pool2;
  for(size_t i = 1; i < cells.size(); i++){ int d = D[cells[i].second][cells[i].first]; if(d > far * 0.35) pool.push_back(cells[i]); if(d > far * 0.12) pool2.push_back(cells[i]); }
  for(int i = 0; i < cfg.relics && !pool.empty(); i++){ int k = rnd((int)pool.size()); items.push_back({'r', cp(pool[k])}); pool.erase(pool.begin() + k); }
  for(int i = 0; i < cfg.batteries && !pool2.empty(); i++){ int k = rnd((int)pool2.size()); items.push_back({'b', cp(pool2[k])}); pool2.erase(pool2.begin() + k); }
  lights.clear(); float sc = (float)cfg.cw / baseCw;                  // spread the fixed lights over bigger mazes
  for(size_t i = 0; i < cfg.lights.size() && i < (size_t)MAXL; i++){
    int cx = std::clamp((int)(cfg.lightCells[i].x * sc), 0, cfg.cw - 1), cy = std::clamp((int)(cfg.lightCells[i].y * sc), 0, cfg.ch - 1);
    lights.push_back({vec3((2 * cx + 1.5f) * S, 2.4f, (2 * cy + 1.5f) * S), cfg.lights[i]});
  }
  nc = std::min({cfg.budget, (int)lights.size(), MAXL});             // P4: the shadow-caster budget is enforced HERE
  std::vector<float> V; std::vector<unsigned> I;                     // open-topped: wall faces that border open cells + floor
  for(int y = 0; y < GH; y++) for(int x = 0; x < GW; x++) if(G[y][x])
    for(int k = 0; k < 4; k++){
      int a = dx4[k], b = dy4[k]; int ny = y + b, nx = x + a; if(ny < 0 || nx < 0 || ny >= GH || nx >= GW || G[ny][nx] != 0) continue;
      float X = (x + (a > 0)) * S, Z = (y + (b > 0)) * S;
      if(a) addQuad(V, I, {X, 0, y * S}, {X, 0, (y + 1) * S}, {X, WH, (y + 1) * S}, {X, WH, y * S}, {(float)a, 0, 0});
      else  addQuad(V, I, {x * S, 0, Z}, {(x + 1) * S, 0, Z}, {(x + 1) * S, WH, Z}, {x * S, WH, Z}, {0, 0, (float)b});
    }
  float X = GW * S, Z = GH * S; addQuad(V, I, {0, 0, 0}, {X, 0, 0}, {X, 0, Z}, {0, 0, Z}, {0, 1, 0});
  mesh.build(V, I, 6, 3);
  start = vec3(1.5f * S, 1.6f, 1.5f * S); startYaw = G[1][2] == 0 ? 0.f : 1.5707963f;
}
