#include "VertexArray.h"
#include "VertexBufferLayout.h"
#include "ErrorManager.h"

VertexArray::VertexArray() : m_Attribute(0)
{
    GLCall(glGenVertexArrays(1, &m_RendererID));
}

VertexArray::~VertexArray()
{
    GLCall(glDeleteVertexArrays(1, &m_RendererID));
}

void VertexArray::AddVBuffer(const VertexBuffer &vb, const VertexBufferLayout &layout)
{
    Bind();
    vb.Bind();
    const auto& elements = layout.GetElements();
    unsigned int offset = 0;
    for (unsigned int i = 0; i < elements.size(); i++)
    {
        const auto& element = elements[i];
        GLCall(glEnableVertexAttribArray(m_Attribute));
        GLCall(glVertexAttribPointer(m_Attribute, element.count, element.type, element.normalized, layout.GetStride(), (const void*)(uintptr_t)offset));
        offset += element.count * VertexBufferElement::GetSizeOfType(element.type);
        m_Attribute++;
    }
    
}

void VertexArray::AddIBuffer(const InstanceBuffer &ivb, const VertexBufferLayout &layout)
{
    Bind();
    ivb.Bind();
    const auto& elements = layout.GetElements();
    unsigned int offset = 0;
    for (unsigned int i = 0; i < elements.size(); i++)
    {
        const auto& element = elements[i];
        GLCall(glEnableVertexAttribArray(m_Attribute));
        GLCall(glVertexAttribPointer(m_Attribute, element.count, element.type, element.normalized, layout.GetStride(), (const void*)(uintptr_t)offset));
        offset += element.count * VertexBufferElement::GetSizeOfType(element.type);
        m_Attribute++;
    }
}

void VertexArray::Bind() const
{
    GLCall(glBindVertexArray(m_RendererID));
}

void VertexArray::Unbind() const
{
    GLCall(glBindVertexArray(0));
}
