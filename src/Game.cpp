#include "Game.h"
#include <algorithm>
#include <cstdio>
using namespace glm;

void Game::prepare(Shadows& sh) { startLevel(sh); state = TITLE; }
void Game::startLevel(Shadows& sh) {
    level.build(levelNo); sh.renderCubes(level); sh.renderMoon(level);   // both static: the walls never move
    cam.pos = level.start; cam.yaw = level.startYaw; cam.pitch = 0.f; bat = 1.f; gtime = 0.f; got = 0; msgT = 0.f; state = PLAY;
    flyMode = false; // <--- ADD THIS
}
void Game::setCasters(int n, Shadows& sh) { level.nc = std::min(n, (int)level.lights.size()); sh.renderCubes(level); }
void Game::handleKey(int key, Shadows& sh, Window& w) {
    if (key == GLFW_KEY_ENTER) { if (state == TITLE) { levelNo = 1; startLevel(sh); } else if (state == WIN) { levelNo++; startLevel(sh); } else if (state == LOSE) startLevel(sh); }
    if (key == GLFW_KEY_R && state != TITLE) startLevel(sh);        // B13: restart without quitting
    if (key == GLFW_KEY_X) shOn = !shOn;                             // break it on purpose
    if (key == GLFW_KEY_C) dynCubes = !dynCubes;                     // inherited behaviour: rebuild point shadows every frame
    if (key >= GLFW_KEY_0 && key <= GLFW_KEY_4) setCasters(key - GLFW_KEY_0, sh);
    if (key == GLFW_KEY_M) { bench = !bench; bf = 0; bacc = 0; w.setVsync(!bench); }
    if (key == GLFW_KEY_P) flyMode = !flyMode; // <--- ADD THIS
}
void Game::update(const Window& w, float dt) {
    if (msgT > 0) msgT -= dt;
    if (state != PLAY) return;
    cam.update(w, dt, [&](float x, float z) { return level.hit(x, z); }, flyMode);
    gtime += dt; bat = std::max(0.f, bat - level.cfg.drain / level.cfg.battery * dt);
    for (auto& it : level.items) {
        if (it.done) continue;
        if (length(it.p - vec2(cam.pos.x, cam.pos.z)) < 1.1f) {
            if (it.t == 'r') { it.done = true; got++; }
            else if (it.t == 'b') { it.done = true; bat = std::min(1.f, bat + 0.25f); }
            else if (got >= level.cfg.relics) state = WIN;
            else { msgT = 2.5f; msgS = "EXIT SEALED - FIND ALL RELICS FIRST"; }
        }
    }
    if (bat <= 0.f && state == PLAY) state = LOSE;
}
void Game::benchTick(double raw) {
    if (!bench) return;
    bacc += raw;
    if (++bf >= 300) {
        double avg = bacc / 300 * 1000; char b[120];
        snprintf(b, 120, "N=%d %s AVG %.2f MS", level.nc, dynCubes ? "DYNAMIC" : "STATIC", avg); benchMsg = b;
        printf("level %d, casters %d, point maps %s, shadows %s: %.3f ms (300 frames)\n", levelNo, level.nc, dynCubes ? "every frame" : "static", shOn ? "on" : "off", avg);
        if (FILE* f = fopen("bench_results.csv", "a")) { fprintf(f, "%d,%d,%s,%s,%.3f\n", levelNo, level.nc, dynCubes ? "dynamic" : "static", shOn ? "on" : "off", avg); fclose(f); }
        bf = 0; bacc = 0;
    }
}
void Game::drawHud(Hud& h, int fbW, int fbH, float ms) {
    h.begin(fbW, fbH);
    float u = fbH / 720.f, x = 20 * u, y = fbH - 48 * u; const vec4 W(1, 1, 1, 0.92f), GOLD(1, 0.8f, 0.2f, 1);
    if (state != TITLE) {
        h.rect(x - 3 * u, y - 3 * u, 236 * u, 26 * u, { 0, 0, 0, 0.6f });
        vec4 bc = bat > 0.5f ? vec4(0.49f, 0.86f, 0.36f, 1) : bat > 0.2f ? vec4(1, 0.7f, 0.28f, 1) : vec4(1, 0.3f, 0.3f, 1);
        h.rect(x, y, 230 * u * bat, 20 * u, bc);
        h.text("BATTERY " + std::to_string((int)(bat * 100)) + "%", x + 6 * u, y + 5 * u, 2.f * u, vec4(0, 0, 0, 0.85f));
        for (int i = 0; i < level.cfg.relics; i++) h.rect(x + i * 26 * u, y - 34 * u, 20 * u, 20 * u, i < got ? GOLD : vec4(0.3f, 0.3f, 0.3f, 0.8f));
        h.text("RELICS " + std::to_string(got) + "/" + std::to_string(level.cfg.relics), x, y - 60 * u, 2.5f * u, W);
        h.text("LEVEL " + std::to_string(levelNo) + "   TIME " + std::to_string((int)gtime), x, y - 86 * u, 2.5f * u, W);
        if (state == PLAY && gtime < 9) h.textC("FIND EVERY RELIC - THEN REACH THE EXIT", 60 * u, 3.f * u, GOLD);
        if (msgT > 0) h.textC(msgS, 100 * u, 3.5f * u, vec4(1, 0.4f, 0.3f, 1));
    }
    h.text(std::to_string((int)(ms * 10) / 10) + "." + std::to_string((int)(ms * 10) % 10) + " MS  CASTERS " + std::to_string(level.nc) + "/" + std::to_string((int)level.lights.size())
        + "  SHADOWS " + (shOn ? "ON" : "OFF") + "  POINT MAPS " + (dynCubes ? "EVERY FRAME" : "STATIC"), 12 * u, 12 * u, 1.6f * u, vec4(1, 1, 1, 0.65f));
    if (bench) h.text("BENCH " + std::to_string(bf) + "/300  " + benchMsg, 12 * u, 32 * u, 1.6f * u, vec4(0.5f, 1, 0.5f, 0.9f));
    if (flyMode) h.text("FLY MODE ACTIVE (P TO DISABLE)", x, y - 112 * u, 2.5f * u, vec4(0.5f, 1.f, 0.5f, 1.f));
    if (state == TITLE) {
        h.rect(0, 0, (float)fbW, (float)fbH, { 0, 0, 0, 0.6f });
        h.textC("DEEP DARK", fbH * 0.58f, 14.f * u, vec4(1, 0.7f, 0.28f, 1));
        h.textC("ONE TORCH. ONE BATTERY. FIND THE RELICS AND GET OUT.", fbH * 0.46f, 3.f * u, W);
        h.textC("PRESS ENTER TO START", fbH * 0.36f, 4.f * u, GOLD);
        h.textC("WASD MOVE  MOUSE LOOK  R RESTART  X SHADOWS  C POINT MAPS  0-4 CASTERS  M BENCH", fbH * 0.22f, 2.f * u, vec4(1, 1, 1, 0.7f));
    }
    if (state == WIN) {
        h.rect(0, 0, (float)fbW, (float)fbH, { 0.1f, 0.8f, 0.3f, 0.3f });
        h.textC("YOU ESCAPED", fbH * 0.55f, 10.f * u, W); h.textC("ENTER NEXT LEVEL    R REPLAY", fbH * 0.4f, 3.5f * u, W);
    }
    if (state == LOSE) {
        h.rect(0, 0, (float)fbW, (float)fbH, { 0.8f, 0.05f, 0.05f, 0.4f });
        h.textC("LOST IN THE DARK", fbH * 0.55f, 10.f * u, W); h.textC("PRESS R TO TRY AGAIN", fbH * 0.4f, 3.5f * u, W);
    }
    h.end();
}