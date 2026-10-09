#include "Input.h"
#include <vector>

namespace vl::Input {

static GLFWwindow* s_Window = nullptr;
static std::vector<unsigned char> s_keysPrev;
static std::vector<unsigned char> s_keysCurr;
static std::vector<unsigned char> s_mousePrev;
static std::vector<unsigned char> s_mouseCurr;
static double s_mouseX = 0.0, s_mouseY = 0.0;
static double s_mouseLastX = 0.0, s_mouseLastY = 0.0;
static bool s_initialized = false;

void Init(GLFWwindow* window) {
	s_Window = window;
	const int keyCount = GLFW_KEY_LAST + 1;
	const int mouseCount = GLFW_MOUSE_BUTTON_LAST + 1;
	s_keysPrev.assign(keyCount, 0);
	s_keysCurr.assign(keyCount, 0);
	s_mousePrev.assign(mouseCount, 0);
	s_mouseCurr.assign(mouseCount, 0);
	if (s_Window) glfwGetCursorPos(s_Window, &s_mouseX, &s_mouseY);
	s_mouseLastX = s_mouseX;
	s_mouseLastY = s_mouseY;
	s_initialized = true;
}

void Shutdown() {
	s_initialized = false;
	s_Window = nullptr;
	s_keysPrev.clear();
	s_keysCurr.clear();
	s_mousePrev.clear();
	s_mouseCurr.clear();
}

void NewFrame() {
	if (!s_initialized || !s_Window) return;

	s_keysPrev = s_keysCurr;
	s_mousePrev = s_mouseCurr;

	for (int k = 0; k <= GLFW_KEY_LAST; ++k)
		s_keysCurr[k] = (glfwGetKey(s_Window, k) == GLFW_PRESS) ? 1 : 0;

	for (int b = 0; b <= GLFW_MOUSE_BUTTON_LAST; ++b)
		s_mouseCurr[b] = (glfwGetMouseButton(s_Window, b) == GLFW_PRESS) ? 1 : 0;

	glfwGetCursorPos(s_Window, &s_mouseX, &s_mouseY);
}

bool IsKeyDown(int glfwKey) {
	if (!s_initialized) return false;
	if (glfwKey < 0 || glfwKey > GLFW_KEY_LAST) return false;
	return s_keysCurr[glfwKey];
}

bool IsKeyPressed(int glfwKey) {
	if (!s_initialized) return false;
	if (glfwKey < 0 || glfwKey > GLFW_KEY_LAST) return false;
	return s_keysCurr[glfwKey] && !s_keysPrev[glfwKey];
}

bool IsKeyReleased(int glfwKey) {
	if (!s_initialized) return false;
	if (glfwKey < 0 || glfwKey > GLFW_KEY_LAST) return false;
	return !s_keysCurr[glfwKey] && s_keysPrev[glfwKey];
}

bool IsMouseDown(int button) {
	if (!s_initialized) return false;
	if (button < 0 || button > GLFW_MOUSE_BUTTON_LAST) return false;
	return s_mouseCurr[button];
}

bool IsMousePressed(int button) {
	if (!s_initialized) return false;
	if (button < 0 || button > GLFW_MOUSE_BUTTON_LAST) return false;
	return s_mouseCurr[button] && !s_mousePrev[button];
}

bool IsMouseReleased(int button) {
	if (!s_initialized) return false;
	if (button < 0 || button > GLFW_MOUSE_BUTTON_LAST) return false;
	return !s_mouseCurr[button] && s_mousePrev[button];
}

void GetMousePos(double& x, double& y) {
	x = s_mouseX;
	y = s_mouseY;
}

void GetMouseDelta(double& dx, double& dy) {
	dx = s_mouseX - s_mouseLastX;
	dy = s_mouseY - s_mouseLastY;
	s_mouseLastX = s_mouseX;
	s_mouseLastY = s_mouseY;
}

bool IsGamepadPresent(int jid) {
	return glfwJoystickPresent(jid) && glfwJoystickIsGamepad(jid);
}

bool GetGamepadButton(int button, int jid) {
	if (!IsGamepadPresent(jid)) return false;
	GLFWgamepadstate state;
	if (glfwGetGamepadState(jid, &state))
		return state.buttons[button] == GLFW_PRESS;
	return false;
}

} // namespace vl::Input
