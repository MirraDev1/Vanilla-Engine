#include "Systems.h"

namespace vl::Scene {

void CameraSystem::Update(Scene& scene, Camera& camera, const vl::Input::InputManager& input, const float deltaTime) const
{
	for (const EntityId entity : scene.GetEntities()) {
		CameraComponent* cameraComponent = scene.GetCamera(entity);
		TransformComponent* transform = scene.GetTransform(entity);
		if (cameraComponent == nullptr || transform == nullptr || !cameraComponent->active) continue;

		camera.SetPosition(transform->position);
		camera.SetRotation(transform->rotation);
		camera.SetFieldOfView(cameraComponent->fieldOfViewDegrees);
		camera.SetClipPlanes(cameraComponent->nearPlane, cameraComponent->farPlane);
		camera.Update(input, deltaTime);
		transform->position = camera.GetPosition();
		transform->rotation = camera.GetRotation();
		return;
	}

	camera.Update(input, deltaTime);
}

} // namespace vl::Scene
