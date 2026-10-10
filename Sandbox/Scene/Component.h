#pragma once
#include <iostream>
#include <string>

class Entity;

class component {
public:
	Entity* entity = nullptr;

	virtual ~component() = default;
	virtual void onUpdate(float deltaTime) {}
	virtual void onStart() {}
	virtual void onDestroy() {}
};