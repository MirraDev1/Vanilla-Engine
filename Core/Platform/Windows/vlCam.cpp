#include "vlCam.h"

#include "Input.h"
#include "Viewport.h"

#include <algorithm>
#include <cmath>

Camera::Camera()
	: viewport_(std::make_unique<Viewport>())
{
}

Camera::~Camera() = default;

DirectX::XMMATRIX Camera::GetViewMatrix() const noexcept
{
	const DirectX::XMVECTOR position = DirectX::XMLoadFloat3(&position_);
	const DirectX::XMFLOAT3 forwardFloat = GetForward();
	const DirectX::XMVECTOR forward = DirectX::XMLoadFloat3(&forwardFloat);
	const DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	return DirectX::XMMatrixLookAtLH(position, DirectX::XMVectorAdd(position, forward), up);
}

DirectX::XMMATRIX Camera::GetProjectionMatrix() const noexcept
{
	return DirectX::XMMatrixPerspectiveFovLH(fieldOfView_, viewport_->AspectRatio(), nearPlane_, farPlane_);
}

void Camera::Update(const vl::Input::InputManager& input, const float deltaTime) noexcept
{
	if (!input.HasViewportInput() || deltaTime <= 0.0f) return;

	if (input.IsMouseDown(GLFW_MOUSE_BUTTON_RIGHT)) {
		double deltaX = 0.0;
		double deltaY = 0.0;
		input.GetMouseDelta(deltaX, deltaY);
		yaw_ += static_cast<float>(deltaX) * mouseSensitivity_;
		pitch_ = std::clamp(pitch_ - static_cast<float>(deltaY) * mouseSensitivity_,
			-DirectX::XM_PIDIV2 + 0.01f, DirectX::XM_PIDIV2 - 0.01f);
	}

	const DirectX::XMFLOAT3 forward = GetForward();
	const DirectX::XMVECTOR forwardVector = DirectX::XMLoadFloat3(&forward);
	const DirectX::XMVECTOR worldUp = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	const DirectX::XMVECTOR rightVector = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(worldUp, forwardVector));
	DirectX::XMVECTOR movement = DirectX::XMVectorZero();
	if (input.IsKeyDown(GLFW_KEY_W)) movement = DirectX::XMVectorAdd(movement, forwardVector);
	if (input.IsKeyDown(GLFW_KEY_S)) movement = DirectX::XMVectorSubtract(movement, forwardVector);
	if (input.IsKeyDown(GLFW_KEY_D)) movement = DirectX::XMVectorAdd(movement, rightVector);
	if (input.IsKeyDown(GLFW_KEY_A)) movement = DirectX::XMVectorSubtract(movement, rightVector);
	if (input.IsKeyDown(GLFW_KEY_E) || input.IsKeyDown(GLFW_KEY_SPACE)) movement = DirectX::XMVectorAdd(movement, worldUp);
	if (input.IsKeyDown(GLFW_KEY_Q) || input.IsKeyDown(GLFW_KEY_LEFT_CONTROL)) movement = DirectX::XMVectorSubtract(movement, worldUp);

	if (DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(movement)) > 0.0f) {
		movement = DirectX::XMVectorScale(DirectX::XMVector3Normalize(movement), movementSpeed_ * deltaTime);
		DirectX::XMStoreFloat3(&position_, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&position_), movement));
	}
}

void Camera::InitViewport(const VlViewport& viewport) { viewport_->VlInitViewport(viewport); }
void Camera::UpdateViewport(const VlViewport& viewport) { viewport_->VlUpdateViewport(viewport); }
void Camera::SetPosition(const DirectX::XMFLOAT3& position) noexcept { position_ = position; }
DirectX::XMFLOAT3 Camera::GetPosition() const noexcept { return position_; }

void Camera::SetRotation(const DirectX::XMFLOAT3& pitchYawRoll) noexcept
{
	pitch_ = std::clamp(pitchYawRoll.x, -DirectX::XM_PIDIV2 + 0.01f, DirectX::XM_PIDIV2 - 0.01f);
	yaw_ = pitchYawRoll.y;
}

DirectX::XMFLOAT3 Camera::GetRotation() const noexcept
{
	return { pitch_, yaw_, 0.0f };
}

void Camera::SetFieldOfView(const float degrees) noexcept
{
	fieldOfView_ = DirectX::XMConvertToRadians(std::clamp(degrees, 1.0f, 179.0f));
}

void Camera::SetClipPlanes(const float nearPlane, const float farPlane) noexcept
{
	if (nearPlane > 0.0f && farPlane > nearPlane) {
		nearPlane_ = nearPlane;
		farPlane_ = farPlane;
	}
}

void Camera::SetMovementSpeed(const float unitsPerSecond) noexcept
{
	if (std::isfinite(unitsPerSecond) && unitsPerSecond > 0.0f) movementSpeed_ = unitsPerSecond;
}

void Camera::SetMouseSensitivity(const float radiansPerPixel) noexcept
{
	if (std::isfinite(radiansPerPixel) && radiansPerPixel > 0.0f) mouseSensitivity_ = radiansPerPixel;
}

DirectX::XMFLOAT3 Camera::GetForward() const noexcept
{
	const float cosinePitch = std::cos(pitch_);
	return { cosinePitch * std::sin(yaw_), std::sin(pitch_), cosinePitch * std::cos(yaw_) };
}






