#pragma once

#include <GLFW/glfw3.h>

namespace vl::Input {

void Init(GLFWwindow* window);
void Shutdown();

// Call once per-frame after glfwPollEvents()
void NewFrame();

bool IsKeyDown(int glfwKey);
bool IsKeyPressed(int glfwKey);
bool IsKeyReleased(int glfwKey);

bool IsMouseDown(int button);
bool IsMousePressed(int button);
bool IsMouseReleased(int button);

void GetMousePos(double& x, double& y);
void GetMouseDelta(double& dx, double& dy);

bool IsGamepadPresent(int jid = GLFW_JOYSTICK_1);
bool GetGamepadButton(int button, int jid = GLFW_JOYSTICK_1);

} // namespace vl::Input
