#include "OpenGLVertexArray.h"

#include "../../render/Buffer.h"
#include "OpenGLBuffer.h"

#include <glad/glad.h>

namespace {
unsigned int ShaderDataTypeToOpenGLBaseType(ShaderDataType type) {
    switch (type) {
    case ShaderDataType::Float:
    case ShaderDataType::Float2:
    case ShaderDataType::Float3:
    case ShaderDataType::Float4:
    case ShaderDataType::Mat3:
    case ShaderDataType::Mat4:
        return GL_FLOAT;
    case ShaderDataType::Int:
    case ShaderDataType::Int2:
    case ShaderDataType::Int3:
    case ShaderDataType::Int4:
        return GL_INT;
    case ShaderDataType::Bool:
        return GL_UNSIGNED_BYTE;
    case ShaderDataType::None:
    default:
        return 0;
    }
}
}

OpenGLVertexArray::OpenGLVertexArray() {
    glGenVertexArrays(1, &m_RendererID);
}

OpenGLVertexArray::~OpenGLVertexArray() {
    glDeleteVertexArrays(1, &m_RendererID);
}

void OpenGLVertexArray::Bind() const {
    glBindVertexArray(m_RendererID);
}

void OpenGLVertexArray::Unbind() const {
    glBindVertexArray(0);
}

void OpenGLVertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) {
    Bind();
    vertexBuffer->Bind();

    const auto& layout = vertexBuffer->GetLayout();
    for (const auto& element : layout) {
        switch (element.Type) {
        case ShaderDataType::Float:
        case ShaderDataType::Float2:
        case ShaderDataType::Float3:
        case ShaderDataType::Float4:
        {
            glEnableVertexAttribArray(m_VertexBufferIndex);
            glVertexAttribPointer(
                m_VertexBufferIndex,
                static_cast<int>(element.GetComponentCount()),
                ShaderDataTypeToOpenGLBaseType(element.Type),
                element.Normalized ? GL_TRUE : GL_FALSE,
                static_cast<int>(layout.GetStride()),
                reinterpret_cast<const void*>(element.Offset)
            );
            ++m_VertexBufferIndex;
            break;
        }
        case ShaderDataType::Int:
        case ShaderDataType::Int2:
        case ShaderDataType::Int3:
        case ShaderDataType::Int4:
        case ShaderDataType::Bool:
        {
            glEnableVertexAttribArray(m_VertexBufferIndex);
            glVertexAttribIPointer(
                m_VertexBufferIndex,
                static_cast<int>(element.GetComponentCount()),
                ShaderDataTypeToOpenGLBaseType(element.Type),
                static_cast<int>(layout.GetStride()),
                reinterpret_cast<const void*>(element.Offset)
            );
            ++m_VertexBufferIndex;
            break;
        }
        case ShaderDataType::Mat3:
        case ShaderDataType::Mat4:
        {
            const unsigned int columnCount = element.GetComponentCount();
            for (unsigned int column = 0; column < columnCount; ++column) {
                glEnableVertexAttribArray(m_VertexBufferIndex);
                glVertexAttribPointer(
                    m_VertexBufferIndex,
                    static_cast<int>(columnCount),
                    ShaderDataTypeToOpenGLBaseType(element.Type),
                    element.Normalized ? GL_TRUE : GL_FALSE,
                    static_cast<int>(layout.GetStride()),
                    reinterpret_cast<const void*>(element.Offset + sizeof(float) * columnCount * column)
                );
                ++m_VertexBufferIndex;
            }
            break;
        }
        case ShaderDataType::None:
        default:
            break;
        }
    }

    m_VertexBuffers.push_back(vertexBuffer);
}

void OpenGLVertexArray::SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer) {
    Bind();
    indexBuffer->Bind();
    m_IndexBuffer = indexBuffer;
}

const std::vector<Ref<VertexBuffer>>& OpenGLVertexArray::GetVertexBuffers() const {
    return m_VertexBuffers;
}

const Ref<IndexBuffer>& OpenGLVertexArray::GetIndexBuffer() const {
    return m_IndexBuffer;
}
