#pragma once
#include <memory>
#include <string>
#include <vector>

class Entity;

class Scene {
public:
	void saveScene(const std::string& filename);
	void LoadScene(const std::string& filename);
private:
	std::vector<std::shared_ptr<Entity>> Entity_;
	vlLogger& logger_;
};
