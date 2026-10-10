#ifndef VL_APPLICATION_H
#define VL_APPLICATION_H
#include <memory>
#include <cstdint>
#include "Platform/Windows/Input.h"
#include "Platform/DirectX/VertexShader.h"
#include "Platform/DirectX/PixelShader.h"

class Camera;

namespace vl::Platform {
class Window;
class Renderer;
}

namespace vl {
class Core;
class DebugLayer;
}

namespace vl::Scene { class Scene; }
namespace vl::Scene { class CameraSystem; }

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
class Application{
public:
    Application();
    ~Application();
	void vlGetEvents();
	int Run();
private:
	std::unique_ptr<vl::Platform::Window> window_;
	std::unique_ptr<vl::Core> core_;
    std::unique_ptr<vl::Platform::Renderer> renderer_;
	std::unique_ptr<vl::UI::UserInterface> userInterface_;
	std::unique_ptr<vl::Shaders::VertexShader> vertexShader_;
	std::unique_ptr<vl::Shaders::PixelShader> pixelShader_;
	std::unique_ptr<vl::DebugLayer>debugLayer_;
	std::unique_ptr<Camera> camera_;
	vl::Input::InputManager inputManager_;
	std::unique_ptr<vl::Scene::Scene> scene_;
	std::uint32_t testTriangleEntity_ = 0;
	std::uint32_t editorCameraEntity_ = 0;
	std::unique_ptr<vl::Scene::CameraSystem> cameraSystem_;
	vshader vertexshader{};
	pshader pixelshader{};
	bool initialized_ = false;
};
}


#endif // VL_APPLICATION_H
