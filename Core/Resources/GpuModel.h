#pragma once

#include "Model.h"

#include <d3d11.h>
#include <wrl/client.h>

#include <string>
#include <vector>

namespace vl::Resources {

class GpuMesh {
public:
	[[nodiscard]] bool Upload(ID3D11Device* device, const Mesh& mesh, std::string& error);
	void Draw(ID3D11DeviceContext* context) const noexcept;
	[[nodiscard]] const Material& GetMaterial() const noexcept { return material_; }
	[[nodiscard]] bool IsReady() const noexcept { return vertexBuffer_ && indexBuffer_ && indexCount_ > 0; }

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer_;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer_;
	Material material_;
	UINT indexCount_ = 0;
};

class GpuModel {
public:
	[[nodiscard]] bool Upload(ID3D11Device* device, const Model& model, std::string& error);
	[[nodiscard]] const std::vector<GpuMesh>& GetMeshes() const noexcept { return meshes_; }

private:
	std::vector<GpuMesh> meshes_;
};

} // namespace vl::Resources
