#ifndef VL_APPLICATION_H
#define VL_APPLICATION_H
#include <memory>
#include "Platform/DirectX/VertexShader.h"
#include "Platform/DirectX/PixelShader.h"


class Camera;

struct Transform;

namespace vl::Platform {
class Window;
class Renderer;
}

namespace vl {
class Core;
class DebugLayer;
}

namespace vl::Shaders {
	class VertexShader;
	class PixelShader;
}

namespace vl::Mesh {
	class Mesh;
}

namespace vl::UI {
class UserInterface;
}

namespace vl::App{
struct ApplicationConfig {
    bool enableEditor = false;
    unsigned int width = 1280;
    unsigned int height = 720;
};

class Application{
public:
    explicit Application(ApplicationConfig config = {});
    ~Application();
	void vlGetEvents();
	int Run();
private:
    ApplicationConfig config_{};
	std::unique_ptr<vl::Platform::Window> window_;
	std::unique_ptr<vl::Core> core_;
    std::unique_ptr<vl::Platform::Renderer> renderer_;
	std::unique_ptr<vl::UI::UserInterface> userInterface_;
	std::unique_ptr<vl::Shaders::VertexShader> vertexShader_;
	std::unique_ptr<vl::Shaders::PixelShader> pixelShader_;
	std::unique_ptr<vl::DebugLayer>debugLayer_;
	std::unique_ptr<Camera> camera_;
	std::unique_ptr<Transform> transform_;
	vshader vertexshader{};
	pshader pixelshader{};
};
}


#endif // VL_APPLICATION_H
