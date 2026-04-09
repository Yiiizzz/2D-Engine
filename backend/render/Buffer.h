#pragma once

#include <initializer_list>
#include <memory>
#include <string>
#include <vector>

#include "RendererAPI.h"

enum class ShaderDataType {
    None = 0,
    Float,
    Float2,
    Float3,
    Float4
};

unsigned int ShaderDataTypeSize(ShaderDataType type);

struct BufferElement {
    std::string Name;
    ShaderDataType Type = ShaderDataType::None;
    unsigned int Size = 0;
    std::size_t Offset = 0;
    bool Normalized = false;

    BufferElement() = default;
    BufferElement(ShaderDataType type, const std::string& name, bool normalized = false);

    unsigned int GetComponentCount() const;
};

class BufferLayout {
public:
    BufferLayout() = default;
    BufferLayout(std::initializer_list<BufferElement> elements);

    const std::vector<BufferElement>& GetElements() const;
    unsigned int GetStride() const;

private:
    void CalculateOffsetsAndStride();

private:
    std::vector<BufferElement> m_Elements;
    unsigned int m_Stride = 0;
};

class VertexBuffer {
public:
    virtual ~VertexBuffer() = default;

    virtual void Bind() const = 0;
    virtual void Unbind() const = 0;
    virtual void SetLayout(const BufferLayout& layout) = 0;
    virtual const BufferLayout& GetLayout() const = 0;

    static std::shared_ptr<VertexBuffer> Create(const float* vertices, unsigned int sizeInBytes);
};

class IndexBuffer {
public:
    virtual ~IndexBuffer() = default;

    virtual void Bind() const = 0;
    virtual void Unbind() const = 0;
    virtual unsigned int GetCount() const = 0;

    static std::shared_ptr<IndexBuffer> Create(const unsigned int* indices, unsigned int count);
};
