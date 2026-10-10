#pragma once

#include "../Resources/Model.h"

#include <DirectXMath.h>

#include <memory>

namespace vl::Scene {

struct TransformComponent {
	DirectX::XMFLOAT3 position{ 0.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT3 rotation{ 0.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT3 scale{ 1.0f, 1.0f, 1.0f };

	[[nodiscard]] DirectX::XMMATRIX GetWorldMatrix() const noexcept;
};

struct MeshComponent {
	std::shared_ptr<const Resources::Model> model;
	bool visible = true;
};

struct CameraComponent {
	float fieldOfViewDegrees = 45.0f;
	float nearPlane = 0.01f;
	float farPlane = 1000.0f;
	bool active = false;
};

} // namespace vl::Scene
