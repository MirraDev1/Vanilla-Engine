#include "Components.h"

namespace vl::Scene {

DirectX::XMMATRIX TransformComponent::GetWorldMatrix() const noexcept
{
	const DirectX::XMMATRIX scaleMatrix = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	const DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
	const DirectX::XMMATRIX translationMatrix = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	// Local-space vertices become world-space positions as S*R*T for DirectX row-vector convention.
	return scaleMatrix * rotationMatrix * translationMatrix;
}

} // namespace vl::Scene
