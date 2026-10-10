#pragma once

#include <GLFW/glfw3.h>

#include <array>

namespace vl::Input {

class InputManager {
public:
	void Init(GLFWwindow* window) noexcept;
	void Shutdown() noexcept;
	void NewFrame(bool wantCaptureKeyboard, bool wantCaptureMouse,
		bool viewportFocused, bool viewportHovered) noexcept;

	[[nodiscard]] bool IsKeyDown(int glfwKey) const noexcept;
	[[nodiscard]] bool IsKeyPressed(int glfwKey) const noexcept;
	[[nodiscard]] bool IsKeyReleased(int glfwKey) const noexcept;
	[[nodiscard]] bool IsMouseDown(int button) const noexcept;
	[[nodiscard]] bool IsMousePressed(int button) const noexcept;
	[[nodiscard]] bool IsMouseReleased(int button) const noexcept;
	void GetMousePos(double& x, double& y) const noexcept;
	void GetMouseDelta(double& dx, double& dy) const noexcept;
	[[nodiscard]] bool HasViewportInput() const noexcept;
	[[nodiscard]] bool IsGamepadPresent(int jid = GLFW_JOYSTICK_1) const noexcept;
	[[nodiscard]] bool GetGamepadButton(int button, int jid = GLFW_JOYSTICK_1) const noexcept;

private:
	GLFWwindow* window_ = nullptr;
	std::array<unsigned char, GLFW_KEY_LAST + 1> keysPrevious_{};
	std::array<unsigned char, GLFW_KEY_LAST + 1> keysCurrent_{};
	std::array<unsigned char, GLFW_MOUSE_BUTTON_LAST + 1> mousePrevious_{};
	std::array<unsigned char, GLFW_MOUSE_BUTTON_LAST + 1> mouseCurrent_{};
	double mouseX_ = 0.0;
	double mouseY_ = 0.0;
	double mouseDeltaX_ = 0.0;
	double mouseDeltaY_ = 0.0;
	double lastMouseX_ = 0.0;
	double lastMouseY_ = 0.0;
	bool viewportInput_ = false;
};

} // namespace vl::Input
