#include "Entity.h"
#include "Component.h"

void Entity::Update(float deltatime){
	for (auto& component : components) {
		component->onUpdate(deltatime);
	}
}
