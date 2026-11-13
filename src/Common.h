#pragma once
// CommonValues.h equivalent. Owner: R1. Nobody else changes it without telling R1.
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <vector>

constexpr float S = 4.f;        // world units per maze cell
constexpr float WH = 3.f;       // wall height
constexpr float FARP = 40.f;    // far plane of every shadow projection
constexpr int CS = 512;         // point-light cube shadow map size, per face
constexpr int TS = 1024;        // torch shadow map size
constexpr int MAPMOON = 2048;   // moon shadow map size
constexpr int MAXL = 4;         // max fixed lights

// Searches the working directory and a few parents, so it runs from the exe folder or the source folder.
bool findAndRead(const std::string& relPath, std::string& out);
