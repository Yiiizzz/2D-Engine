#include "OpenGLContext.h"

#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>

OpenGLContext::OpenGLContext(void* windowHandle)
    : m_WindowHandle(static_cast<GLFWwindow*>(windowHandle)) {
}

void OpenGLContext::Init() {
    if (m_WindowHandle == nullptr) {
        throw std::runtime_error("OpenGLContext requires a valid GLFW window.");
    }

    glfwMakeContextCurrent(m_WindowHandle);

    if (gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)) == 0) {
        throw std::runtime_error("gladLoadGLLoader failed.");
    }

    std::cout << "OpenGL Vendor: " << reinterpret_cast<const char*>(glGetString(GL_VENDOR)) << std::endl;
    std::cout << "OpenGL Renderer: " << reinterpret_cast<const char*>(glGetString(GL_RENDERER)) << std::endl;
    std::cout << "OpenGL Version: " << reinterpret_cast<const char*>(glGetString(GL_VERSION)) << std::endl;
}

void OpenGLContext::SwapBuffers() {
    glfwSwapBuffers(m_WindowHandle);
}
