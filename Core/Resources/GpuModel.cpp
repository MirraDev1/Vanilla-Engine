#include "GpuModel.h"

#include <limits>
#include <utility>

namespace vl::Resources {

bool GpuMesh::Upload(ID3D11Device* device, const Mesh& mesh, std::string& error)
{
	error.clear();
	if (device == nullptr) {
		error = "Cannot upload a mesh without a Direct3D device";
		return false;
	}
	if (mesh.vertices.empty() || mesh.indices.empty()) {
		error = "Cannot upload a mesh without vertices and indices";
		return false;
	}
	if (mesh.vertices.size() > (std::numeric_limits<UINT>::max)() / sizeof(Vertex) ||
		mesh.indices.size() > (std::numeric_limits<UINT>::max)() / sizeof(std::uint32_t) ||
		mesh.indices.size() > (std::numeric_limits<UINT>::max)()) {
		error = "Mesh exceeds Direct3D 11 buffer size limits";
		return false;
	}

	D3D11_BUFFER_DESC vertexDescription{};
	vertexDescription.ByteWidth = static_cast<UINT>(mesh.vertices.size() * sizeof(Vertex));
	vertexDescription.Usage = D3D11_USAGE_IMMUTABLE;
	vertexDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	D3D11_SUBRESOURCE_DATA vertexData{};
	vertexData.pSysMem = mesh.vertices.data();
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	HRESULT result = device->CreateBuffer(&vertexDescription, &vertexData, vertexBuffer.GetAddressOf());
	if (FAILED(result)) {
		error = "Failed to create Direct3D vertex buffer (HRESULT " + std::to_string(static_cast<unsigned long>(result)) + ")";
		return false;
	}

	D3D11_BUFFER_DESC indexDescription{};
	indexDescription.ByteWidth = static_cast<UINT>(mesh.indices.size() * sizeof(std::uint32_t));
	indexDescription.Usage = D3D11_USAGE_IMMUTABLE;
	indexDescription.BindFlags = D3D11_BIND_INDEX_BUFFER;
	D3D11_SUBRESOURCE_DATA indexData{};
	indexData.pSysMem = mesh.indices.data();
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
	result = device->CreateBuffer(&indexDescription, &indexData, indexBuffer.GetAddressOf());
	if (FAILED(result)) {
		error = "Failed to create Direct3D index buffer (HRESULT " + std::to_string(static_cast<unsigned long>(result)) + ")";
		return false;
	}

	vertexBuffer_ = std::move(vertexBuffer);
	indexBuffer_ = std::move(indexBuffer);
	indexCount_ = static_cast<UINT>(mesh.indices.size());
	material_ = mesh.material;
	return true;
}

void GpuMesh::Draw(ID3D11DeviceContext* context) const noexcept
{
	if (context == nullptr || !IsReady()) return;
	ID3D11Buffer* vertexBuffer = vertexBuffer_.Get();
	const UINT stride = sizeof(Vertex);
	const UINT offset = 0;
	context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	context->IASetIndexBuffer(indexBuffer_.Get(), DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->DrawIndexed(indexCount_, 0, 0);
}

bool GpuModel::Upload(ID3D11Device* device, const Model& model, std::string& error)
{
	error.clear();
	if (model.meshes.empty()) {
		error = "Cannot upload a model without meshes";
		return false;
	}
	std::vector<GpuMesh> uploadedMeshes;
	uploadedMeshes.reserve(model.meshes.size());
	for (const Mesh& mesh : model.meshes) {
		GpuMesh gpuMesh;
		if (!gpuMesh.Upload(device, mesh, error)) return false;
		uploadedMeshes.push_back(std::move(gpuMesh));
	}
	meshes_ = std::move(uploadedMeshes);
	return true;
}

} // namespace vl::Resources
