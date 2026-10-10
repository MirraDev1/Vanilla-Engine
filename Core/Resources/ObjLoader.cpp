#include "ObjLoader.h"

#include <tiny_obj_loader.h>

#include <algorithm>
#include <limits>
#include <unordered_map>
#include <utility>

namespace vl::Resources {
namespace {

bool ReadPosition(const tinyobj::attrib_t& attributes, const tinyobj::index_t& index, DirectX::XMFLOAT3& position)
{
	if (index.vertex_index < 0) return false;
	const std::size_t offset = static_cast<std::size_t>(index.vertex_index) * 3;
	if (offset + 2 >= attributes.vertices.size()) return false;
	position = { attributes.vertices[offset], attributes.vertices[offset + 1], attributes.vertices[offset + 2] };
	return true;
}

bool ReadNormal(const tinyobj::attrib_t& attributes, const tinyobj::index_t& index, DirectX::XMFLOAT3& normal)
{
	if (index.normal_index < 0) return false;
	const std::size_t offset = static_cast<std::size_t>(index.normal_index) * 3;
	if (offset + 2 >= attributes.normals.size()) return false;
	normal = { attributes.normals[offset], attributes.normals[offset + 1], attributes.normals[offset + 2] };
	return true;
}

bool ReadUv(const tinyobj::attrib_t& attributes, const tinyobj::index_t& index, DirectX::XMFLOAT2& uv)
{
	if (index.texcoord_index < 0) return false;
	const std::size_t offset = static_cast<std::size_t>(index.texcoord_index) * 2;
	if (offset + 1 >= attributes.texcoords.size()) return false;
	uv = { attributes.texcoords[offset], 1.0f - attributes.texcoords[offset + 1] };
	return true;
}

DirectX::XMFLOAT3 FaceNormal(const DirectX::XMFLOAT3& a, const DirectX::XMFLOAT3& b, const DirectX::XMFLOAT3& c)
{
	const DirectX::XMVECTOR edgeA = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&b), DirectX::XMLoadFloat3(&a));
	const DirectX::XMVECTOR edgeB = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&c), DirectX::XMLoadFloat3(&a));
	const DirectX::XMVECTOR cross = DirectX::XMVector3Cross(edgeA, edgeB);
	const float lengthSquared = DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(cross));
	if (lengthSquared <= 1.0e-12f) return { 0.0f, 1.0f, 0.0f };
	DirectX::XMFLOAT3 normal{};
	DirectX::XMStoreFloat3(&normal, DirectX::XMVector3Normalize(cross));
	return normal;
}

Material ConvertMaterial(const tinyobj::material_t& source, const std::filesystem::path& directory)
{
	Material result;
	result.name = source.name;
	result.diffuseColor = { source.diffuse[0], source.diffuse[1], source.diffuse[2], source.dissolve };
	if (!source.diffuse_texname.empty()) result.diffuseTexture = directory / source.diffuse_texname;
	return result;
}

} // namespace

bool ObjLoader::Load(const std::filesystem::path& path, Model& model, std::string& error)
{
	model = {};
	error.clear();
	if (path.empty()) {
		error = "OBJ path is empty";
		return false;
	}
	if (!std::filesystem::is_regular_file(path)) {
		error = "OBJ file does not exist: " + path.string();
		return false;
	}

	tinyobj::attrib_t attributes;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warning;
	const std::string filename = path.string();
	const std::string materialDirectory = path.parent_path().string();
	if (!tinyobj::LoadObj(&attributes, &shapes, &materials, &warning, &error,
		filename.c_str(), materialDirectory.empty() ? nullptr : materialDirectory.c_str(), true)) {
		if (!warning.empty()) error = warning + error;
		return false;
	}

	model.sourcePath = path;
	for (const tinyobj::shape_t& shape : shapes) {
		std::unordered_map<int, std::size_t> meshByMaterial;
		std::size_t indexOffset = 0;
		for (std::size_t face = 0; face < shape.mesh.num_face_vertices.size(); ++face) {
			const std::size_t faceVertexCount = shape.mesh.num_face_vertices[face];
			const int materialId = face < shape.mesh.material_ids.size() ? shape.mesh.material_ids[face] : -1;
			auto [found, inserted] = meshByMaterial.try_emplace(materialId, model.meshes.size());
			if (inserted) {
				Mesh mesh;
				mesh.name = shape.name;
				if (materialId >= 0 && static_cast<std::size_t>(materialId) < materials.size())
					mesh.material = ConvertMaterial(materials[materialId], path.parent_path());
				else
					mesh.material.name = "Default";
				model.meshes.push_back(std::move(mesh));
			}
			Mesh& mesh = model.meshes[found->second];
			std::vector<Vertex> faceVertices;
			std::vector<bool> hasNormals;
			faceVertices.reserve(faceVertexCount);
			hasNormals.reserve(faceVertexCount);
			for (std::size_t corner = 0; corner < faceVertexCount; ++corner) {
				if (indexOffset + corner >= shape.mesh.indices.size()) {
					error = "OBJ face index stream is malformed";
					model = {};
					return false;
				}
				const tinyobj::index_t& sourceIndex = shape.mesh.indices[indexOffset + corner];
				Vertex vertex{};
				if (!ReadPosition(attributes, sourceIndex, vertex.position)) {
					error = "OBJ contains an invalid position index";
					model = {};
					return false;
				}
				hasNormals.push_back(ReadNormal(attributes, sourceIndex, vertex.normal));
				ReadUv(attributes, sourceIndex, vertex.uv);
				faceVertices.push_back(vertex);
			}
			for (std::size_t triangle = 1; triangle + 1 < faceVertices.size(); ++triangle) {
				const std::size_t corners[3] = { 0, triangle, triangle + 1 };
				const DirectX::XMFLOAT3 fallbackNormal = FaceNormal(faceVertices[0].position,
					faceVertices[triangle].position, faceVertices[triangle + 1].position);
				for (const std::size_t corner : corners) {
					if (mesh.vertices.size() >= std::numeric_limits<std::uint32_t>::max()) {
						error = "OBJ mesh exceeds the supported 32-bit index range";
						model = {};
						return false;
					}
					Vertex vertex = faceVertices[corner];
					if (!hasNormals[corner]) vertex.normal = fallbackNormal;
					mesh.indices.push_back(static_cast<std::uint32_t>(mesh.vertices.size()));
					mesh.vertices.push_back(vertex);
				}
			}
			indexOffset += faceVertexCount;
		}
	}
	model.meshes.erase(std::remove_if(model.meshes.begin(), model.meshes.end(),
		[](const Mesh& mesh) { return mesh.indices.empty(); }), model.meshes.end());
	if (model.meshes.empty()) {
		error = "OBJ contains no renderable faces";
		model = {};
		return false;
	}
	return true;
}

} // namespace vl::Resources
