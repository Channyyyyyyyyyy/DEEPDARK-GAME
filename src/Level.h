#pragma once
// Owner: R1 (the scene format and loader)
#include "Common.h"
#include "Mesh.h"
struct Item { char t; glm::vec2 p; bool done = false; };   // 'r' relic, 'b' battery, 'e' exit
struct Light { glm::vec3 p, c; };
struct Config {
  int cw = 12, ch = 9, seed = 7, relics = 3, batteries = 5, budget = 2;
  float battery = 100, drain = 0.55f, amb = 0.02f;
  std::vector<glm::vec3> lights; std::vector<glm::vec2> lightCells;
  glm::vec3 mA[6]; glm::vec2 mS[6];
  glm::vec3 tatt{0.3f, 0.09f, 0.02f}, patt{0.3f, 0.2f, 0.1f}, mdir{-0.35f, -1.f, -0.25f}, mcol{0.05f, 0.07f, 0.14f};
  Config();
};
Config parseLevel(const std::string& text);   // reports the line number of anything it cannot read

class Level {
public:
  Config cfg; std::vector<std::vector<int>> G; int GW = 0, GH = 0;
  std::vector<Item> items; std::vector<Light> lights; int nc = 0;   // nc = how many fixed lights cast shadows
  Mesh mesh; glm::vec3 start{0.f}; float startYaw = 0.f;
  void build(int levelNo);       // re-reads level.txt every time, then scales difficulty by levelNo
  bool solidAt(float x, float z) const;
  bool hit(float x, float z) const;   // the player is a circle-ish box of radius 0.35
};
