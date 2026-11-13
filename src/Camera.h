#pragma once
// Owner: R4 (movement is gameplay)
#include "Common.h"
#include "Window.h"
#include <functional>
class Camera {
public:
	glm::vec3 pos{ 0.f, 1.6f, 0.f }; float yaw = 0.f, pitch = 0.f;
	glm::vec3 forward() const;
	glm::mat4 view() const;
	glm::mat4 proj(float aspect) const;
	glm::mat4 torchVP() const;   // one 40-degree perspective map for the 20-degree-half-angle torch cone (E2)
	// B4: all movement is scaled by dt, so speed does not depend on the frame rate.
	void update(const Window& w, float dt, const std::function<bool(float, float)>& blocked, bool flyMode);
};