#pragma once

#include "ErrorManager.h"
#include <string>
#include <vector>

class Texture
{
public:
    Texture();
    ~Texture();

    void Bind(unsigned int slot = 0) const;
    void Unbind() const;

    void PathGenerate(const std::string &path);
    void DataGenerate();
    void ImageData(const std::string &path);

    inline int GetWidth() const { return m_Width; }
    inline int GetHeight() const { return m_Height; }
private:
    std::vector<unsigned int> m_RendererID;
    std::string m_FilePath;
    unsigned char* m_LocalBuffer;
    int m_Width, m_Height, m_BPP, m_Count;
};