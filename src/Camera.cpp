#include "Camera.h"
#include <cmath>
using namespace glm;
vec3 Camera::forward() const { return vec3(std::cos(yaw) * std::cos(pitch), std::sin(pitch), std::sin(yaw) * std::cos(pitch)); }
mat4 Camera::view() const { return lookAt(pos, pos + forward(), vec3(0, 1, 0)); }
mat4 Camera::proj(float aspect) const { return perspective(radians(70.f), aspect, 0.1f, 100.f); }
mat4 Camera::torchVP() const { return perspective(radians(40.f), 1.f, 0.2f, FARP) * view(); }
void Camera::update(const Window& w, float dt, const std::function<bool(float, float)>& blocked, bool flyMode) {
    yaw += (float)w.mdx * 0.0025f; pitch = glm::clamp(pitch - (float)w.mdy * 0.0025f, -1.2f, 1.2f);
    if (w.key(GLFW_KEY_LEFT)) yaw -= 1.8f * dt; if (w.key(GLFW_KEY_RIGHT)) yaw += 1.8f * dt;

    if (flyMode) {
        // --- FLY MODE ---
        vec3 fwd = forward(); // includes looking up/down
        vec3 right = normalize(cross(fwd, vec3(0, 1, 0)));
        vec3 move(0.f);

        if (w.key(GLFW_KEY_W)) move += fwd;
        if (w.key(GLFW_KEY_S)) move -= fwd;
        if (w.key(GLFW_KEY_D)) move += right;
        if (w.key(GLFW_KEY_A)) move -= right;
        if (w.key(GLFW_KEY_SPACE)) move += vec3(0, 1, 0); // Fly Up
        if (w.key(GLFW_KEY_LEFT_SHIFT)) move -= vec3(0, 1, 0); // Fly Down

        if (length(move) > 0.f) {
            pos += normalize(move) * 12.f * dt; // Fly faster (12 units/sec)
        }
    }
    else {
        // --- NORMAL WALKING ---
        float fx = std::cos(yaw), fz = std::sin(yaw), mx = 0, mz = 0;
        if (w.key(GLFW_KEY_W)) { mx += fx; mz += fz; } if (w.key(GLFW_KEY_S)) { mx -= fx; mz -= fz; }
        if (w.key(GLFW_KEY_D)) { mx -= fz; mz += fx; } if (w.key(GLFW_KEY_A)) { mx += fz; mz -= fx; }
        float l = std::hypot(mx, mz); if (l > 0) { float sp = 4.5f * dt; mx = mx / l * sp; mz = mz / l * sp; }
        if (!blocked(pos.x + mx, pos.z)) pos.x += mx;
        if (!blocked(pos.x, pos.z + mz)) pos.z += mz;
    }
}