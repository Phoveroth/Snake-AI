#pragma once

#include "Header.h"

class Render
{
public:
protected:
    void Draw(const VertexArray &va, const IndexBuffer &ib, const Shader &shader);
    void DrawInstanced(const VertexArray &va, const IndexBuffer &ib, const Shader &shader, GLsizei count);
    void DrawLines(const VertexArray &va, const IndexBuffer &ib, const Shader &shader);

    std::unique_ptr<VertexArray> m_VAO;
    std::unique_ptr<VertexBuffer> m_VertexBuffer;
    std::unique_ptr<InstanceBuffer> m_InstanceBuffer;
    std::unique_ptr<IndexBuffer> m_IndexBuffer;
    std::unique_ptr<Shader> m_Shader;
    std::unique_ptr<Texture> m_Texture;

    GLFWwindow* m_Window;
    WindowData* m_Data;
    UIData* m_UIData;
    GameData* m_GameData;
    glm::mat4 m_Proj;
private:
};