#pragma once
#include <memory>
#include <string>
#include <vector>

class component;

class Entity {
public:
	//Add Components To the Entity
	std::string name;
	uint64_t EntityID;
	std::vector<std::shared_ptr<component>> components;
	template<typename T, typename... Args>
	T& addComponent(Args&&... args) {
		auto component = std::make_shared<T>(std::forward<Args>(args)...);
		component->entity = this;
		component.push_back(component);

		return *component;
	}
	//Get Components From the Entity to Modify Them
	template<typename T>
	T& getComponent() {
		for (auto& component : components) {
			if (auto* casted = std::dynamic_cast<T*>(component.get())) {
				return casted;
			}
		}
		return nullptr;
	}

	template<typename T>
	bool HasComponent() {
		for (auto& component : components) {
			if (auto* casted = std::dynamic_cast<T*>(component.get())) {
				return true;
			}
		}
		return false;
	}

	void Update(float deltatime);
	uint64_t GetEntityID() const { return EntityID; }

};
