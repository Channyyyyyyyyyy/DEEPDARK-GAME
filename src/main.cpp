// DEEP DARK - Group 5. Owner: R1. The frame loop only; everything else lives in its own file.
#include "Common.h"
#include "Window.h"
#include "Game.h"
#include "Shadows.h"
#include "Renderer.h"
#include "Sky.h"
#include "Hud.h"
#include <algorithm>
#include <cstdio>
using namespace glm;

int main(){
  Window win; if(!win.create(1280, 720, "DEEP DARK")) return 1;
  Shadows shadows; Renderer renderer; Sky sky; Hud hud; Game game;
  if(!shadows.init() || !renderer.init() || !sky.init() || !hud.init()){ fprintf(stderr, "Initialisation failed (see messages above).\n"); return 1; }
  game.prepare(shadows);                                    // title screen over a prepared level
  double last = glfwGetTime(), acc = 0; int frames = 0; float ms = 16.f;
  while(!win.shouldClose()){
    // ---- 1. input and time
    win.poll(); double now = glfwGetTime(), raw = now - last; float dt = (float)std::min(raw, 0.1); last = now;   // B4
    acc += dt; if(++frames == 60){ ms = (float)(acc / 60 * 1000); acc = 0; frames = 0; }
    for(int k : win.pressed) game.handleKey(k, shadows, win);
    // ---- 2. game logic
    game.benchTick(raw); game.update(win, dt);
    // ---- 3. shadow passes (point-light and moon maps are static unless C is pressed)
    if(game.dynCubes) shadows.renderCubes(game.level);
    mat4 tvp = game.cam.torchVP(); shadows.renderTorch(game.level, tvp);
    // ---- 4. scene, sky, HUD
    glBindFramebuffer(GL_FRAMEBUFFER, 0); glViewport(0, 0, win.width, win.height); glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    mat4 view = game.cam.view(), proj = game.cam.proj(win.aspect());
    renderer.draw(game, shadows, view, proj, tvp, now);
    sky.draw(view, proj, normalize(game.level.cfg.mdir));
    game.drawHud(hud, win.width, win.height, ms);
    if(game.bench) glFinish();
    win.swap();
  }
  glfwTerminate(); return 0;
}
