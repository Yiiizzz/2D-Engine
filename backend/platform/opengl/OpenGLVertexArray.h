#pragma once

#include "../../render/VertexArray.h"

#include <vector>

class OpenGLVertexArray : public VertexArray {
public:
    OpenGLVertexArray();
    ~OpenGLVertexArray() override;

    void Bind() const override;
    void Unbind() const override;
    void AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) override;
    void SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer) override;
    const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() const override;
    const Ref<IndexBuffer>& GetIndexBuffer() const override;

private:
    unsigned int m_RendererID = 0;
    unsigned int m_VertexBufferIndex = 0;
    std::vector<Ref<VertexBuffer>> m_VertexBuffers;
    Ref<IndexBuffer> m_IndexBuffer;
};
