#pragma once
// Owner: R4 - the mechanic, the state machine, collision use, the HUD contents.
#include "Common.h"
#include "Camera.h"
#include "Level.h"
#include "Shadows.h"
#include "Hud.h"
class Game {
public:
	enum State { PLAY = 0, WIN = 1, LOSE = 2, TITLE = 3 };
	State state = TITLE; int levelNo = 1;
	Level level; Camera cam;
	float bat = 1.f, gtime = 0.f; int got = 0;
	bool shOn = true, dynCubes = false, bench = false;
	bool flyMode = false; // <--- ADD THIS
	float msgT = 0.f; std::string msgS, benchMsg; int bf = 0; double bacc = 0;
	void prepare(Shadows& sh);                            // build level 1 behind the title screen
	void startLevel(Shadows& sh);
	void handleKey(int key, Shadows& sh, Window& w);
	void update(const Window& w, float dt);
	void benchTick(double rawDt);                         // M: average 300 frames, print, append to bench_results.csv
	void setCasters(int n, Shadows& sh);                  // P1: sweep N live
	void drawHud(Hud& h, int w, int hgt, float ms);
};