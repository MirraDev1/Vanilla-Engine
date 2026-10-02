#include "vlCam.h"
#include "imgui.h"
#include "Viewport.h"
#include <cmath>

Camera::Camera()
	: viewport_(std::make_unique<Viewport>())
{
	camera_.Position = { 0.0f, 0.0f, -5.0f};
	camera_.Up = { 0.0f, 1.0f, 0.0f };
	float pitch = XMConvertToRadians(userCamera_.m_pitch);
	float yaw = XMConvertToRadians(userCamera_.m_yaw);
	camera_.Foward = {
		std::cos(pitch) * std::sin(yaw),
		std::sin(pitch),
		std::cos(pitch) * std::cos(yaw)
	};
}

Camera::~Camera()
{
}

XMMATRIX Camera::GetViewMatrix()
{
	 m_Pos = XMLoadFloat3(&camera_.Position);
	 m_Up = XMLoadFloat3(&camera_.Up);
     m_Foward = XMLoadFloat3(&camera_.Foward);
	 UpdateViewMatrix();
	return XMMatrixLookAtLH(m_Pos, XMVectorAdd(m_Pos, m_Foward), m_Up);
}

XMMATRIX Camera::GetProjectionMatrix()
{
	return XMMatrixPerspectiveFovLH(XM_PIDIV4, viewport_->AspectRatio(), 0.01f, 100.0f);
}

void Camera::InitViewport(const VlViewport& viewport)
{
	viewport_->VlInitViewport(viewport);
}

void Camera::UpdateViewport(const VlViewport& viewport)
{
	viewport_->VlUpdateViewport(viewport);
}

void Camera::UpdateViewMatrix(){
	//1. Gotta Calculate the right vector
	m_Foward = XMLoadFloat3(&camera_.Foward);
	m_Up = XMLoadFloat3(&camera_.Up);

	m_right = XMVector3Normalize(XMVector3Cross(m_Up, m_Foward));
	XMStoreFloat3(&camera_.Right, m_right);

	//2. The true Up vector
	XMVECTOR Up = XMLoadFloat3(&camera_.Up);
	Up = XMVector3Normalize(XMVector3Cross(m_Foward, m_right));
	m_Up = Up;
	XMStoreFloat3(&camera_.Up, Up);
}






