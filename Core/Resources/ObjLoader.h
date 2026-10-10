#pragma once

#include "Model.h"

#include <string>

namespace vl::Resources {

class ObjLoader {
public:
	[[nodiscard]] static bool Load(const std::filesystem::path& path, Model& model, std::string& error);
};

} // namespace vl::Resources
