#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"
#include <fstream>

Texture::Texture()
    : m_LocalBuffer(nullptr),
    m_Width(0), m_Height(0), m_BPP(0), m_Count(0)
{
    m_RendererID.reserve(5);
}

Texture::~Texture()
{
    GLCall(glDeleteTextures(m_Count + 1, m_RendererID.data()));
}

void Texture::Bind(unsigned int slot) const
{
    GLCall(glActiveTexture(GL_TEXTURE0 + slot));
    GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID.at(slot)));
}

void Texture::Unbind() const
{
    GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}

void Texture::PathGenerate(const std::string &path)
{
    m_FilePath = path;
    if (m_RendererID.size() <= m_Count)
        m_RendererID.resize(m_Count + 5);

    stbi_set_flip_vertically_on_load(1);
    m_LocalBuffer = stbi_load(path.c_str(), &m_Width, &m_Height, &m_BPP, 4);

    GLCall(glGenTextures(1, m_RendererID.data() + m_Count));
    GLCall(glActiveTexture(GL_TEXTURE0 + m_Count));
    GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID.at(m_Count)));

    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

    GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_LocalBuffer))
    GLCall(glBindTexture(GL_TEXTURE_2D, 0));

    m_Count++;

    if (m_LocalBuffer)
        stbi_image_free(m_LocalBuffer);
}

void Texture::DataGenerate()
{
    // Change LocalBuffer m_Width m_Height and stuff
    if (m_RendererID.size() <= m_Count)
        m_RendererID.resize(m_Count + 5);

    GLCall(glGenTextures(1, m_RendererID.data() + m_Count));
    GLCall(glActiveTexture(GL_TEXTURE0 + m_Count));
    GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID.at(m_Count)));

    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

    GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_LocalBuffer))
    GLCall(glBindTexture(GL_TEXTURE_2D, 0));

    m_Count++;

    if (m_LocalBuffer)
        stbi_image_free(m_LocalBuffer);
}

void Texture::ImageData(const std::string &path)
{
    int width;
    int height;
    stbi_set_flip_vertically_on_load(1);
    m_LocalBuffer = stbi_load(path.c_str(), &width, &height, &m_BPP, 4);

    std::ofstream headerFile ("imageData.h");

    headerFile << "// Image dimensions\n";
    headerFile << "const int " << "width = " << width << ";\n";
    headerFile << "const int " << "height = " << height << ";\n";
    headerFile << "// Pixel data\n";
    headerFile << "const unsigned char " << "data[] = {";

    for (int i = 0; i < width * height * 4; ++i) {
        headerFile << "0x";
        if ((int)m_LocalBuffer[i] == 0)
        {
            headerFile << "00"; // purely style
        } else
        {
            headerFile << std::hex << (int)m_LocalBuffer[i];
        }
        
        if (i < width * height * 4 - 1) {
            headerFile << ", ";
        }
    }
    headerFile << "};";

    if (m_LocalBuffer)
        stbi_image_free(m_LocalBuffer);
}
