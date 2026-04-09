#pragma once

#include "../../render/GraphicsContext.h"

struct GLFWwindow;

class OpenGLContext : public GraphicsContext {
public:
    explicit OpenGLContext(void* windowHandle);

    void Init() override;
    void SwapBuffers() override;

private:
    GLFWwindow* m_WindowHandle = nullptr;
};
