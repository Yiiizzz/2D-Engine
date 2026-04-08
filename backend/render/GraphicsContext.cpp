#include "GraphicsContext.h"

#include "../platform/opengl/OpenGLContext.h"

#include <stdexcept>

std::unique_ptr<GraphicsContext> GraphicsContext::Create(void* windowHandle, GraphicsAPI api) {
    switch (api) {
    case GraphicsAPI::OpenGL:
        return std::make_unique<OpenGLContext>(windowHandle);
    case GraphicsAPI::Vulkan:
    case GraphicsAPI::DX12:
    case GraphicsAPI::None:
    default:
        throw std::runtime_error("Requested graphics backend is not implemented yet.");
    }
}
