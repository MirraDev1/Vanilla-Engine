#include "ComponentFactory.h"
#include "Component.h"

void ComponentFactory::Register(const std::string& type, CreateFunc createfunc){
	GetRegistry()[type] = createfunc;
}

std::shared_ptr<component> ComponentFactory::createComponent(const std::string& type){
	auto it = GetRegistry().find(type);
	if(it != GetRegistry().end()){
		return it->second();
	}
	return nullptr;
}