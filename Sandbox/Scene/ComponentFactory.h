#pragma once
#include <memory>
#include <iostream>
#include <unordered_map>
#include <string>
#include <functional>


class component;

class ComponentFactory {
public:
	using CreateFunc = std::function<std::shared_ptr<component>()>;
	static void Register(const std::string& type, CreateFunc createfunc);
	static std::shared_ptr<component> createComponent(const std::string& type);
private:
	static std::unordered_map<std::string, CreateFunc>& GetRegistry() {
		static std::unordered_map<std::string, CreateFunc> registry;
		return registry;
	}
};