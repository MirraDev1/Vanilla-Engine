#pragma once

#include <DirectXMath.h>

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

namespace vl::Resources {

struct Vertex {
	DirectX::XMFLOAT3 position{ 0.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT3 normal{ 0.0f, 1.0f, 0.0f };
	DirectX::XMFLOAT2 uv{ 0.0f, 0.0f };
};

struct Material {
	std::string name;
	DirectX::XMFLOAT4 diffuseColor{ 1.0f, 1.0f, 1.0f, 1.0f };
	std::filesystem::path diffuseTexture;
};

struct Mesh {
	std::string name;
	Material material;
	std::vector<Vertex> vertices;
	std::vector<std::uint32_t> indices;
};

struct Model {
	std::filesystem::path sourcePath;
	std::vector<Mesh> meshes;
};

} // namespace vl::Resources
