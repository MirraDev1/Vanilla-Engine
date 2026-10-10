#include "vlScene.h"
#include "Core/Vendor/nlohmann/json.hpp"
#include "Entity.h"
#include "Component.h"
#include <fstream>
#include "Core/Platform/DirectX/Logger.h"

typedef nlohmann::json json;

void Scene::saveScene(const std::string& filename){
	json sceneJson;
	std::ofstream Jsonfstream(filename);
	//Loop through Entities and save their data
	for (const auto& m_entities : Entity_) {
		json entityJson;
		entityJson["EntityID"] = m_entities->GetEntityID();
		entityJson["Name"] = m_entities->name;

		for (auto& component : m_entities->components) {
			entityJson["Components"].push_back(component->toJson());
		}

		sceneJson["Entities"].push_back(entityJson);
	}

	if (Jsonfstream.is_open()) {
    	Jsonfstream << sceneJson.dump(4);
	}
	Jsonfstream.close();
}

void Scene::LoadScene(const std::string& filename){
	std::ifstream Jsonfstream(filename);
	
	json inSceneJson;
	if(!Jsonfstream.is_open()){
		logger_.Warning("Scene", "Failed to open scene file: " + filename);
		return;
	}
	Jsonfstream >> inSceneJson;
	Jsonfstream.close();

	Entity_.clear(); // Clear existing entities before loading new ones

	for(const auto& entityJson : inSceneJson["Entities"]){
		auto m_entity = std::make_shared<Entity>();
		m_entity->name = entityJson["Name"].get<std::string>();
		m_entity->EntityID = entityJson["EntityID"].get<uint64_t>();

		for(const auto& componentJson : entityJson["Components"]){
			std::string componentType = componentJson["Type"].get<std::string>();
			auto component = ComponentFactory::createComponent(componentType);
			if(component){
				component->fromJson(componentJson);
				m_entity->components.push_back(component);
			}else{
				logger_.Warning("Scene", "Unknown component type: " + componentType);
			}
		}
		Entity_.push_back(m_entity);
	}
}
