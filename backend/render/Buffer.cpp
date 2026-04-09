#include "Buffer.h"

#include "../platform/opengl/OpenGLBuffer.h"

#include <stdexcept>

unsigned int ShaderDataTypeSize(ShaderDataType type) {
    switch (type) {
    case ShaderDataType::Float:
        return 4;
    case ShaderDataType::Float2:
        return 4 * 2;
    case ShaderDataType::Float3:
        return 4 * 3;
    case ShaderDataType::Float4:
        return 4 * 4;
    case ShaderDataType::Mat3:
        return 4 * 3 * 3;
    case ShaderDataType::Mat4:
        return 4 * 4 * 4;
    case ShaderDataType::Int:
        return 4;
    case ShaderDataType::Int2:
        return 4 * 2;
    case ShaderDataType::Int3:
        return 4 * 3;
    case ShaderDataType::Int4:
        return 4 * 4;
    case ShaderDataType::Bool:
        return 1;
    case ShaderDataType::None:
    default:
        return 0;
    }
}

BufferElement::BufferElement(ShaderDataType type, const std::string& name, bool normalized)
    : Name(name),
      Type(type),
      Size(ShaderDataTypeSize(type)),
      Normalized(normalized) {
}

unsigned int BufferElement::GetComponentCount() const {
    switch (Type) {
    case ShaderDataType::Float:
        return 1;
    case ShaderDataType::Float2:
        return 2;
    case ShaderDataType::Float3:
        return 3;
    case ShaderDataType::Float4:
        return 4;
    case ShaderDataType::Mat3:
        return 3;
    case ShaderDataType::Mat4:
        return 4;
    case ShaderDataType::Int:
        return 1;
    case ShaderDataType::Int2:
        return 2;
    case ShaderDataType::Int3:
        return 3;
    case ShaderDataType::Int4:
        return 4;
    case ShaderDataType::Bool:
        return 1;
    case ShaderDataType::None:
    default:
        return 0;
    }
}

BufferLayout::BufferLayout(std::initializer_list<BufferElement> elements)
    : m_Elements(elements) {
    CalculateOffsetsAndStride();
}

const std::vector<BufferElement>& BufferLayout::GetElements() const {
    return m_Elements;
}

unsigned int BufferLayout::GetStride() const {
    return m_Stride;
}

std::vector<BufferElement>::const_iterator BufferLayout::begin() const {
    return m_Elements.begin();
}

std::vector<BufferElement>::const_iterator BufferLayout::end() const {
    return m_Elements.end();
}

void BufferLayout::CalculateOffsetsAndStride() {
    std::size_t offset = 0;
    m_Stride = 0;

    for (auto& element : m_Elements) {
        element.Offset = offset;
        offset += element.Size;
        m_Stride += element.Size;
    }
}

Ref<VertexBuffer> VertexBuffer::Create(const float* vertices, unsigned int sizeInBytes) {
    switch (RendererAPI::GetAPI()) {
    case GraphicsAPI::OpenGL:
        return CreateRef<OpenGLVertexBuffer>(vertices, sizeInBytes);
    case GraphicsAPI::Vulkan:
    case GraphicsAPI::DX12:
    case GraphicsAPI::None:
    default:
        throw std::runtime_error("VertexBuffer backend is not implemented yet.");
    }
}

Ref<IndexBuffer> IndexBuffer::Create(const unsigned int* indices, unsigned int count) {
    switch (RendererAPI::GetAPI()) {
    case GraphicsAPI::OpenGL:
        return CreateRef<OpenGLIndexBuffer>(indices, count);
    case GraphicsAPI::Vulkan:
    case GraphicsAPI::DX12:
    case GraphicsAPI::None:
    default:
        throw std::runtime_error("IndexBuffer backend is not implemented yet.");
    }
}
