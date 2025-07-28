#pragma once

#include "VertexBuffer.h"
#include "InstanceBuffer.h"

class VertexBufferLayout;

class VertexArray
{
private:
    unsigned int m_RendererID;
    unsigned int m_Attribute;
public:
    VertexArray();
    ~VertexArray();

    void AddVBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout);
    void AddIBuffer(const InstanceBuffer& ivb, const VertexBufferLayout& layout);

    void Bind() const;
    void Unbind() const;
};