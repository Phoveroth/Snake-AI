#include "Network.h"

Network::Network(GLFWwindow* window, WindowData* data, UIData* uidata, GameData* gamedata, Life* snakedata) :
    m_SnakeData(snakedata)
{
    m_Window = window;
    m_Data = data;
    m_UIData = uidata;
    m_GameData = gamedata;
    m_Proj = data ->projection;

    float vertex[] = {
    //  Positions
                         0.0f,                  0.0f, // Bottom Left
        m_GameData->grid_size,                  0.0f, // Bottom Right
        m_GameData->grid_size, m_GameData->grid_size, // Top Right
                         0.0f, m_GameData->grid_size, // Top Left
    };

    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    int NodeCount = (INPUT_LAYER * sizeof(Input)) + (FIRST_LAYER * sizeof(Input))

    m_VAO = std::make_unique<VertexArray>();
    m_VertexBuffer = std::make_unique<VertexBuffer>(vertex, sizeof(vertex), GL_STATIC_DRAW);
    m_InstanceBuffer = std::make_unique<InstanceBuffer>();
    m_InstanceBuffer->Data(m_SnakeData, m_SnakeData.size() * sizeof(glm::vec4), GL_DYNAMIC_DRAW);

    VertexBufferLayout vblayout;
    VertexBufferLayout ivblayout;

    vblayout.Push(GL_FLOAT, 2);  // VertexCoords
    ivblayout.Push(GL_FLOAT, 2); // IsActive
    ivblayout.Push(GL_FLOAT, 1);
    ivblayout.Push(GL_FLOAT, 1);

    m_VAO->AddVBuffer(*m_VertexBuffer, vblayout);
    m_VAO->AddIBuffer(*m_InstanceBuffer, ivblayout);
    GLCall(glVertexAttribDivisor(2, 1);)
    GLCall(glVertexAttribDivisor(3, 1);)
    GLCall(glVertexAttribDivisor(4, 1);)

    m_IndexBuffer = std::make_unique<IndexBuffer>(indices, 6);
    m_Shader = std::make_unique<Shader>("../../res/shaders/Snake.shader");
    m_Shader->Bind();
    m_Texture = std::make_unique<Texture>();
    m_Texture->PathGenerate("../../res/textures/SnakeAtlas.png");
    m_Texture->Bind(0);
    m_Shader->SetUniform1i("u_textureatlas", 0);
    glDisable(GL_BLEND);
}