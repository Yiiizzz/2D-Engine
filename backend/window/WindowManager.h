#pragma once

#include <memory>
#include <string>

#include "../render/GraphicsAPI.h"
#include "../render/GraphicsContext.h"

struct GLFWwindow;

struct WindowSpecification {
    std::string Title = "Lancelot Engine";
    unsigned int Width = 1280;
    unsigned int Height = 720;
    bool VSync = true;
    GraphicsAPI API = GraphicsAPI::OpenGL;
};

class WindowManager {
public:
    WindowManager() = default;
    ~WindowManager();

    bool Init(const WindowSpecification& specification);
    void Shutdown();
    void PollEvents();
    void OnUpdate();
    bool ShouldClose() const;
    void ToggleFullscreen();
    void Resize(int width, int height);
    void SetVSync(bool enabled);
    bool IsVSync() const;
    unsigned int GetWidth() const;
    unsigned int GetHeight() const;
    GraphicsAPI GetGraphicsAPI() const;
    GLFWwindow* GetNativeWindow() const;

private:
    static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);

private:
    WindowSpecification m_Specification;
    GLFWwindow* m_Window = nullptr;
    std::unique_ptr<GraphicsContext> m_Context;
    bool m_Initialized = false;
    bool m_Fullscreen = false;
    int m_WindowedX = 100;
    int m_WindowedY = 100;
    int m_WindowedWidth = 1280;
    int m_WindowedHeight = 720;
};
