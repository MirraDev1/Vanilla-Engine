#include "vlWindow.h"

#include <iostream>

namespace vl::Platform {
    Window::~Window() {
        if (vlwindow_ != nullptr) {
            glfwDestroyWindow(vlwindow_);
        }
        if (glfwInitialized_) {
            glfwTerminate();
        }
    }

    bool Window::vlCreateWindow(const std::uint32_t width, const std::uint32_t height, const std::string_view title) {
        if (!glfwInit()) {
            std::cerr << "VL: Failed to initialize GLFW.\n";
            return false;
        }
        glfwInitialized_ = true;
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        vlwindow_ = glfwCreateWindow(static_cast<int>(width), static_cast<int>(height), title.data(), nullptr, nullptr);
        if (vlwindow_ == nullptr) {
            std::cerr << "VL: Failed to create the window.\n";
            return false;
        }

        glfwSetWindowUserPointer(vlwindow_, this);
        glfwSetFramebufferSizeCallback(vlwindow_, &Window::FramebufferSizeCallback);

        int framebufferWidth = 0;
        int framebufferHeight = 0;
        glfwGetFramebufferSize(vlwindow_, &framebufferWidth, &framebufferHeight);
        width_ = static_cast<std::uint32_t>(framebufferWidth);
        height_ = static_cast<std::uint32_t>(framebufferHeight);
        return true;
    }

    bool Window::ConsumeResize(std::uint32_t& width, std::uint32_t& height) noexcept {
        if (!resizePending_) {
            return false;
        }

        width = width_;
        height = height_;
        resizePending_ = false;
        return true;
    }

    void Window::FramebufferSizeCallback(GLFWwindow* window, const int width, const int height) {
        auto* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
        if (self == nullptr) {
            return;
        }

        self->width_ = width > 0 ? static_cast<std::uint32_t>(width) : 0;
        self->height_ = height > 0 ? static_cast<std::uint32_t>(height) : 0;
        self->resizePending_ = true;
    }
}
