#pragma once

#include "Scene.h"
#include "../Platform/Windows/Input.h"
#include "../Platform/Windows/vlCam.h"

namespace vl::Scene {

class CameraSystem {
public:
	void Update(Scene& scene, Camera& camera, const vl::Input::InputManager& input, float deltaTime) const;
};

} // namespace vl::Scene
