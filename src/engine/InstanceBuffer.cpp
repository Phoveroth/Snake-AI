#include "InstanceBuffer.h"

InstanceBuffer::InstanceBuffer()
{
    GLCall(glGenBuffers(1, &m_RendererID));
    Bind();
}

InstanceBuffer::~InstanceBuffer()
{
    GLCall(glDeleteBuffers(1, &m_RendererID));
}

void InstanceBuffer::Data(const void *data, unsigned int size, unsigned int usage)
{
    Bind();
    GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, usage));
}

void InstanceBuffer::SubData(const void *data, unsigned int size, unsigned int offset)
{
    Bind();
    GLCall(glBufferSubData(GL_ARRAY_BUFFER, offset, size, data));
}

void InstanceBuffer::Bind() const
{
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
}

void InstanceBuffer::Unbind() const
{
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}
