#pragma once

#include "../../render/Buffer.h"

class OpenGLVertexBuffer : public VertexBuffer {
public:
    OpenGLVertexBuffer(const float* vertices, unsigned int sizeInBytes);
    ~OpenGLVertexBuffer() override;

    void Bind() const override;
    void Unbind() const override;
    void SetLayout(const BufferLayout& layout) override;
    const BufferLayout& GetLayout() const override;

private:
    unsigned int m_RendererID = 0;
    BufferLayout m_Layout;
};

class OpenGLIndexBuffer : public IndexBuffer {
public:
    OpenGLIndexBuffer(const unsigned int* indices, unsigned int count);
    ~OpenGLIndexBuffer() override;

    void Bind() const override;
    void Unbind() const override;
    unsigned int GetCount() const override;

private:
    unsigned int m_RendererID = 0;
    unsigned int m_Count = 0;
};
