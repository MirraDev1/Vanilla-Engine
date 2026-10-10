#pragma once

#include "Components.h"

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace vl::Scene {

using EntityId = std::uint32_t;

class Scene {
public:
	[[nodiscard]] EntityId CreateEntity(std::string name);
	[[nodiscard]] bool DestroyEntity(EntityId entity) noexcept;
	[[nodiscard]] bool Contains(EntityId entity) const noexcept;
	[[nodiscard]] std::vector<EntityId> GetEntities() const;
	[[nodiscard]] const std::string* GetName(EntityId entity) const noexcept;
	[[nodiscard]] bool SetName(EntityId entity, std::string name);

	TransformComponent& AddTransform(EntityId entity, TransformComponent component = {});
	MeshComponent& AddMesh(EntityId entity, MeshComponent component = {});
	CameraComponent& AddCamera(EntityId entity, CameraComponent component = {});
	[[nodiscard]] bool HasTransform(EntityId entity) const noexcept;
	[[nodiscard]] bool HasMesh(EntityId entity) const noexcept;
	[[nodiscard]] bool HasCamera(EntityId entity) const noexcept;
	[[nodiscard]] TransformComponent* GetTransform(EntityId entity) noexcept;
	[[nodiscard]] const TransformComponent* GetTransform(EntityId entity) const noexcept;
	[[nodiscard]] MeshComponent* GetMesh(EntityId entity) noexcept;
	[[nodiscard]] const MeshComponent* GetMesh(EntityId entity) const noexcept;
	[[nodiscard]] CameraComponent* GetCamera(EntityId entity) noexcept;
	[[nodiscard]] const CameraComponent* GetCamera(EntityId entity) const noexcept;
	[[nodiscard]] bool RemoveMesh(EntityId entity) noexcept;
	[[nodiscard]] bool RemoveCamera(EntityId entity) noexcept;

private:
	EntityId nextEntity_ = 1;
	std::unordered_map<EntityId, std::string> names_;
	std::unordered_map<EntityId, TransformComponent> transforms_;
	std::unordered_map<EntityId, MeshComponent> meshes_;
	std::unordered_map<EntityId, CameraComponent> cameras_;
};

} // namespace vl::Scene
