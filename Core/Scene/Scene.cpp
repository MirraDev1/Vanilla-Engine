#include "Scene.h"

#include <algorithm>
#include <limits>
#include <stdexcept>
#include <utility>

namespace vl::Scene {

EntityId Scene::CreateEntity(std::string name)
{
	if (nextEntity_ == 0 || nextEntity_ == std::numeric_limits<EntityId>::max()) {
		throw std::overflow_error("Scene entity ID space exhausted");
	}
	const EntityId entity = nextEntity_++;
	names_.emplace(entity, name.empty() ? "Entity" : std::move(name));
	return entity;
}

bool Scene::DestroyEntity(const EntityId entity) noexcept
{
	if (names_.erase(entity) == 0) return false;
	transforms_.erase(entity);
	meshes_.erase(entity);
	cameras_.erase(entity);
	return true;
}

bool Scene::Contains(const EntityId entity) const noexcept { return names_.contains(entity); }

std::vector<EntityId> Scene::GetEntities() const
{
	std::vector<EntityId> result;
	result.reserve(names_.size());
	for (const auto& [entity, name] : names_) {
		static_cast<void>(name);
		result.push_back(entity);
	}
	std::sort(result.begin(), result.end());
	return result;
}

const std::string* Scene::GetName(const EntityId entity) const noexcept
{
	const auto found = names_.find(entity);
	return found == names_.end() ? nullptr : &found->second;
}

bool Scene::SetName(const EntityId entity, std::string name)
{
	const auto found = names_.find(entity);
	if (found == names_.end()) return false;
	found->second = name.empty() ? "Entity" : std::move(name);
	return true;
}

TransformComponent& Scene::AddTransform(const EntityId entity, TransformComponent component)
{
	if (!Contains(entity)) throw std::invalid_argument("Cannot add a component to an unknown entity");
	return transforms_.insert_or_assign(entity, std::move(component)).first->second;
}

MeshComponent& Scene::AddMesh(const EntityId entity, MeshComponent component)
{
	if (!Contains(entity)) throw std::invalid_argument("Cannot add a component to an unknown entity");
	return meshes_.insert_or_assign(entity, std::move(component)).first->second;
}

CameraComponent& Scene::AddCamera(const EntityId entity, CameraComponent component)
{
	if (!Contains(entity)) throw std::invalid_argument("Cannot add a component to an unknown entity");
	return cameras_.insert_or_assign(entity, std::move(component)).first->second;
}

bool Scene::HasTransform(const EntityId entity) const noexcept { return transforms_.contains(entity); }
bool Scene::HasMesh(const EntityId entity) const noexcept { return meshes_.contains(entity); }
bool Scene::HasCamera(const EntityId entity) const noexcept { return cameras_.contains(entity); }
TransformComponent* Scene::GetTransform(const EntityId entity) noexcept
{
	const auto found = transforms_.find(entity);
	return found == transforms_.end() ? nullptr : &found->second;
}
const TransformComponent* Scene::GetTransform(const EntityId entity) const noexcept
{
	const auto found = transforms_.find(entity);
	return found == transforms_.end() ? nullptr : &found->second;
}
MeshComponent* Scene::GetMesh(const EntityId entity) noexcept
{
	const auto found = meshes_.find(entity);
	return found == meshes_.end() ? nullptr : &found->second;
}
const MeshComponent* Scene::GetMesh(const EntityId entity) const noexcept
{
	const auto found = meshes_.find(entity);
	return found == meshes_.end() ? nullptr : &found->second;
}
CameraComponent* Scene::GetCamera(const EntityId entity) noexcept
{
	const auto found = cameras_.find(entity);
	return found == cameras_.end() ? nullptr : &found->second;
}
const CameraComponent* Scene::GetCamera(const EntityId entity) const noexcept
{
	const auto found = cameras_.find(entity);
	return found == cameras_.end() ? nullptr : &found->second;
}
bool Scene::RemoveMesh(const EntityId entity) noexcept { return meshes_.erase(entity) != 0; }
bool Scene::RemoveCamera(const EntityId entity) noexcept { return cameras_.erase(entity) != 0; }

} // namespace vl::Scene
