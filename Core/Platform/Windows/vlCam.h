#pragma once
#include <cstdint>
#include <DirectXMath.h>
#include <d3d11.h>
#include <wrl/client.h>
#include <memory>

using namespace Microsoft::WRL;
using namespace DirectX;

struct Viewport;
struct VlViewport;

struct Matrix {
	XMMATRIX m_world;
	XMMATRIX m_view;
	XMMATRIX m_projection;
};

struct userCam {
	bool Isopen = false;
	float m_pitch{};
	float m_yaw{};
	float mouse_y{};
	float mouse_x{};
};

struct vlCam {
	XMFLOAT3 Position;
	XMFLOAT3 Up;
	XMFLOAT3 Foward;
	XMFLOAT3 Right;
};

class Camera {
public:
	Camera();
	~Camera();
	XMMATRIX GetViewMatrix();
	XMMATRIX GetProjectionMatrix();
	void UpdateViewMatrix();
	void InitViewport(const VlViewport& viewport);
	void UpdateViewport(const VlViewport& viewport);
private:
	std::unique_ptr<Viewport> viewport_;
	vlCam camera_{};
	userCam userCamera_{};
	XMVECTOR m_Pos{};
	XMVECTOR m_Up{};
	XMVECTOR m_Foward{};
	XMVECTOR m_right{};
};
