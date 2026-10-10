#include "./Sandbox/Scene/Component.h"
#include <DirectXMath.h>

using namespace DirectX;

struct Transform : public component {
	XMFLOAT3 Position;
	XMFLOAT3 Rotation;
	XMFLOAT3 Scaling;
};
