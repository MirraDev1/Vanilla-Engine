#pragma once
#include <DirectXMath.h>
#include <memory>

namespace vl::Input { class InputManager; }
struct VlViewport;
class Viewport;

class Camera {
public:
	Camera();
	~Camera();
	[[nodiscard]] DirectX::XMMATRIX GetViewMatrix() const noexcept;
	[[nodiscard]] DirectX::XMMATRIX GetProjectionMatrix() const noexcept;
	void Update(const vl::Input::InputManager& input, float deltaTime) noexcept;
	void InitViewport(const VlViewport& viewport);
	void UpdateViewport(const VlViewport& viewport);
	void SetPosition(const DirectX::XMFLOAT3& position) noexcept;
	[[nodiscard]] DirectX::XMFLOAT3 GetPosition() const noexcept;
	void SetRotation(const DirectX::XMFLOAT3& pitchYawRoll) noexcept;
	[[nodiscard]] DirectX::XMFLOAT3 GetRotation() const noexcept;
	void SetFieldOfView(float degrees) noexcept;
	void SetClipPlanes(float nearPlane, float farPlane) noexcept;
	void SetMovementSpeed(float unitsPerSecond) noexcept;
	void SetMouseSensitivity(float radiansPerPixel) noexcept;
private:
	[[nodiscard]] DirectX::XMFLOAT3 GetForward() const noexcept;
	std::unique_ptr<Viewport> viewport_;
	DirectX::XMFLOAT3 position_{ 0.0f, 0.0f, -5.0f };
	float yaw_ = 0.0f;
	float pitch_ = 0.0f;
	float fieldOfView_ = DirectX::XM_PIDIV4;
	float nearPlane_ = 0.01f;
	float farPlane_ = 1000.0f;
	float movementSpeed_ = 5.0f;
	float mouseSensitivity_ = 0.0025f;
};
