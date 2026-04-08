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
        return GL_FLOAT;
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

void OpenGLVertexArray::AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer) {
    Bind();
    vertexBuffer->Bind();

    const auto& layout = vertexBuffer->GetLayout();
    for (const auto& element : layout.GetElements()) {
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
    }

    m_VertexBuffers.push_back(vertexBuffer);
}

void OpenGLVertexArray::SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) {
    Bind();
    indexBuffer->Bind();
    m_IndexBuffer = indexBuffer;
}

const std::vector<std::shared_ptr<VertexBuffer>>& OpenGLVertexArray::GetVertexBuffers() const {
    return m_VertexBuffers;
}

const std::shared_ptr<IndexBuffer>& OpenGLVertexArray::GetIndexBuffer() const {
    return m_IndexBuffer;
}
