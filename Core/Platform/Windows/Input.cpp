#include "Input.h"

namespace vl::Input {

void InputManager::Init(GLFWwindow* window) noexcept
{
	window_ = window;
	keysPrevious_.fill(0);
	keysCurrent_.fill(0);
	mousePrevious_.fill(0);
	mouseCurrent_.fill(0);
	mouseDeltaX_ = 0.0;
	mouseDeltaY_ = 0.0;
	viewportInput_ = false;
	if (window_ != nullptr) {
		glfwGetCursorPos(window_, &mouseX_, &mouseY_);
		lastMouseX_ = mouseX_;
		lastMouseY_ = mouseY_;
	}
}

void InputManager::Shutdown() noexcept
{
	window_ = nullptr;
	keysPrevious_.fill(0);
	keysCurrent_.fill(0);
	mousePrevious_.fill(0);
	mouseCurrent_.fill(0);
	mouseDeltaX_ = 0.0;
	mouseDeltaY_ = 0.0;
	viewportInput_ = false;
}

void InputManager::NewFrame(const bool wantCaptureKeyboard, const bool wantCaptureMouse,
	const bool viewportFocused, const bool viewportHovered) noexcept
{
	if (window_ == nullptr) return;

	keysPrevious_ = keysCurrent_;
	mousePrevious_ = mouseCurrent_;
	viewportInput_ = viewportFocused && viewportHovered;
	const bool allowKeyboard = !wantCaptureKeyboard;
	const bool allowMouse = viewportInput_ || !wantCaptureMouse;

	for (int key = 0; key <= GLFW_KEY_LAST; ++key)
		keysCurrent_[key] = allowKeyboard && glfwGetKey(window_, key) == GLFW_PRESS ? 1 : 0;
	for (int button = 0; button <= GLFW_MOUSE_BUTTON_LAST; ++button)
		mouseCurrent_[button] = allowMouse && glfwGetMouseButton(window_, button) == GLFW_PRESS ? 1 : 0;

	glfwGetCursorPos(window_, &mouseX_, &mouseY_);
	mouseDeltaX_ = allowMouse ? mouseX_ - lastMouseX_ : 0.0;
	mouseDeltaY_ = allowMouse ? mouseY_ - lastMouseY_ : 0.0;
	lastMouseX_ = mouseX_;
	lastMouseY_ = mouseY_;
}

bool InputManager::IsKeyDown(const int key) const noexcept
{
	return key >= 0 && key <= GLFW_KEY_LAST && keysCurrent_[key] != 0;
}

bool InputManager::IsKeyPressed(const int key) const noexcept
{
	return key >= 0 && key <= GLFW_KEY_LAST && keysCurrent_[key] && !keysPrevious_[key];
}

bool InputManager::IsKeyReleased(const int key) const noexcept
{
	return key >= 0 && key <= GLFW_KEY_LAST && !keysCurrent_[key] && keysPrevious_[key];
}

bool InputManager::IsMouseDown(const int button) const noexcept
{
	return button >= 0 && button <= GLFW_MOUSE_BUTTON_LAST && mouseCurrent_[button] != 0;
}

bool InputManager::IsMousePressed(const int button) const noexcept
{
	return button >= 0 && button <= GLFW_MOUSE_BUTTON_LAST && mouseCurrent_[button] && !mousePrevious_[button];
}

bool InputManager::IsMouseReleased(const int button) const noexcept
{
	return button >= 0 && button <= GLFW_MOUSE_BUTTON_LAST && !mouseCurrent_[button] && mousePrevious_[button];
}

void InputManager::GetMousePos(double& x, double& y) const noexcept
{
	x = mouseX_;
	y = mouseY_;
}

void InputManager::GetMouseDelta(double& dx, double& dy) const noexcept
{
	dx = mouseDeltaX_;
	dy = mouseDeltaY_;
}

bool InputManager::HasViewportInput() const noexcept
{
	return viewportInput_;
}

bool InputManager::IsGamepadPresent(const int jid) const noexcept
{
	return glfwJoystickPresent(jid) && glfwJoystickIsGamepad(jid);
}

bool InputManager::GetGamepadButton(const int button, const int jid) const noexcept
{
	if (!IsGamepadPresent(jid) || button < 0 || button > GLFW_GAMEPAD_BUTTON_LAST) return false;
	GLFWgamepadstate state{};
	return glfwGetGamepadState(jid, &state) && state.buttons[button] == GLFW_PRESS;
}

} // namespace vl::Input
