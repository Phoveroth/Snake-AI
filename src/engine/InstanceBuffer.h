#pragma once

#include "ErrorManager.h"

class InstanceBuffer
{
private:
    unsigned int m_RendererID;
public:
    InstanceBuffer();
    ~InstanceBuffer();

    void Data(const void* data, unsigned int size, unsigned int usage = GL_STATIC_DRAW);
    void SubData(const void* data, unsigned int size, unsigned int offset);
    void Bind() const;
    void Unbind() const;
};