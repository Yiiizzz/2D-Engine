#include "WindowManager.h"

#include "../render/GraphicsContext.h"

#include <GLFW/glfw3.h>

#include <iostream>

WindowManager::~WindowManager() {
    Shutdown();
}

bool WindowManager::Init(const WindowSpecification& specification) {
    m_Specification = specification;

    if (glfwInit() == GLFW_FALSE) {
        std::cerr << "glfwInit failed." << std::endl;
        return false;
    }

    if (m_Specification.API == GraphicsAPI::OpenGL) {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    }

    m_Window = glfwCreateWindow(
        static_cast<int>(m_Specification.Width),
        static_cast<int>(m_Specification.Height),
        m_Specification.Title.c_str(),
        nullptr,
        nullptr
    );

    if (m_Window == nullptr) {
        std::cerr << "glfwCreateWindow failed." << std::endl;
        glfwTerminate();
        return false;
    }

    glfwSetWindowUserPointer(m_Window, this);
    glfwSetFramebufferSizeCallback(m_Window, FramebufferSizeCallback);

    m_Context = GraphicsContext::Create(m_Window, m_Specification.API);
    m_Context->Init();
    SetVSync(m_Specification.VSync);
    m_Initialized = true;
    return true;
}

void WindowManager::Shutdown() {
    if (!m_Initialized && m_Window == nullptr) {
        return;
    }

    m_Context.reset();

    if (m_Window != nullptr) {
        glfwDestroyWindow(m_Window);
        m_Window = nullptr;
    }

    glfwTerminate();
    m_Initialized = false;
}

void WindowManager::PollEvents() {
    glfwPollEvents();
}

void WindowManager::OnUpdate() {
    if (m_Context) {
        m_Context->SwapBuffers();
    }
}

bool WindowManager::ShouldClose() const {
    return m_Window == nullptr || glfwWindowShouldClose(m_Window) != 0;
}

void WindowManager::SetVSync(bool enabled) {
    glfwSwapInterval(enabled ? 1 : 0);
    m_Specification.VSync = enabled;
}

bool WindowManager::IsVSync() const {
    return m_Specification.VSync;
}

unsigned int WindowManager::GetWidth() const {
    return m_Specification.Width;
}

unsigned int WindowManager::GetHeight() const {
    return m_Specification.Height;
}

GraphicsAPI WindowManager::GetGraphicsAPI() const {
    return m_Specification.API;
}

GLFWwindow* WindowManager::GetNativeWindow() const {
    return m_Window;
}

void WindowManager::FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
    auto* windowManager = static_cast<WindowManager*>(glfwGetWindowUserPointer(window));
    if (windowManager == nullptr) {
        return;
    }

    windowManager->m_Specification.Width = width > 0 ? static_cast<unsigned int>(width) : 1U;
    windowManager->m_Specification.Height = height > 0 ? static_cast<unsigned int>(height) : 1U;
}
