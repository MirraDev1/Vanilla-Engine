#include "Core/Application.h"
#include "Core/Platform/Windows/vlProscess.h"

int main(int argc, char* argv[]) {    
    std::filesystem::path projectPath;
	std::filesystem::path scenePath;

	for(int i = 0; i < argc; ++i) {
		std::string arg = argv[i];
		if(arg == "--project" && i + 1 < argc) {
			projectPath = argv[++i];
		} else if(arg == "--scene" && i + 1 < argc) {
			scenePath = argv[++i];
		}
	}

	if (!projectPath.empty() && !scenePath.empty()) {
		vl::App::ApplicationConfig config;
		config.enableEditor = false; //We don't need an editor at this point

		vl::App::Application app(config);
		return app.Run();
	}
}
