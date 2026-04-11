#include "GraphicsContext.h"

#include "../platform/opengl/OpenGLContext.h"

#include <stdexcept>

Scope<GraphicsContext> GraphicsContext::Create(void* windowHandle, GraphicsAPI api) {
    switch (api) {
    case GraphicsAPI::OpenGL:
        return CreateScope<OpenGLContext>(windowHandle);
    case GraphicsAPI::Vulkan:
    case GraphicsAPI::DX12:
    case GraphicsAPI::None:
    default:
        throw std::runtime_error("Requested graphics backend is not implemented yet.");
    }
}
