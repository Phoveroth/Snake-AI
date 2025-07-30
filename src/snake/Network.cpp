#include "Network.h"

Network::Network(GLFWwindow* window, WindowData* data, UIData* uidata, GameData* gamedata)
{
    m_Window = window;
    m_Data = data;
    m_UIData = uidata;
    m_GameData = gamedata;
    m_Proj = data->projection;
    m_CubeSize = m_GameData->grid_size / 2;

    float vertex[] = {
    //  Positions
        0.0f,       0.0f,       // Bottom Left
        m_CubeSize, 0.0f,       // Bottom Right
        m_CubeSize, m_CubeSize, // Top Right
        0.0f,       m_CubeSize, // Top Left
    };

    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    Flush();

    m_VAO = std::make_unique<VertexArray>();
    m_VertexBuffer = std::make_unique<VertexBuffer>(vertex, sizeof(vertex), GL_STATIC_DRAW);
    m_InstanceBuffer = std::make_unique<InstanceBuffer>();
    m_InstanceBuffer->Data(m_NodeDatas, m_NodeCount * sizeof(NodeData), GL_DYNAMIC_DRAW);

    VertexBufferLayout vblayout;
    VertexBufferLayout ivblayout;

    vblayout.Push(GL_FLOAT, 2);  // VertexCoords
    ivblayout.Push(GL_FLOAT, 1); // HowActive
    ivblayout.Push(GL_FLOAT, 2); // Coords

    m_VAO->AddVBuffer(*m_VertexBuffer, vblayout);
    m_VAO->AddIBuffer(*m_InstanceBuffer, ivblayout);
    GLCall(glVertexAttribDivisor(2, 1);)
    GLCall(glVertexAttribDivisor(3, 1);)

    m_IndexBuffer = std::make_unique<IndexBuffer>(indices, 6);
    m_Shader = std::make_unique<Shader>("../../res/shaders/Network.shader");
    m_Shader->Bind();
}

Network::~Network()
{
}

void Network::OnUpdate(float deltaTime)
{
}

void Network::OnRender()
{   
    m_InstanceBuffer->Data(m_NodeDatas, m_NodeCount * sizeof(NodeData), GL_DYNAMIC_DRAW);

    m_Shader->Bind();
    m_Shader->SetUniform2f("u_translate", m_UIData->TranslateNetwork.x, m_UIData->TranslateNetwork.y);
    m_Shader->SetUniformMat4f("u_proj", m_Data->projection);

    DrawInstanced(*m_VAO, *m_IndexBuffer, *m_Shader, m_NodeCount);
}

void Network::Flush()
{
    int id = 0;

    for (int i = 0; i < INPUT_LAYER; i++)
    {
        m_NodeDatas[id].activation = 0.0;
        m_NodeDatas[id].y = i * 2 * m_CubeSize;
        m_NodeDatas[id].x = 0.0;
        id++;
    }

    for (int i = 0; i < FIRST_LAYER; i++)
    {
        m_NodeDatas[id].activation = 0.0;
        m_NodeDatas[id].y = i * 2 * m_CubeSize + 8 * m_CubeSize;
        m_NodeDatas[id].x = 1 * 5 * m_CubeSize;
        id++;
    }

    for (int i = 0; i < SECOND_LAYER; i++)
    {
        m_NodeDatas[id].activation = 0.0;
        m_NodeDatas[id].y = i * 2 * m_CubeSize + 12 * m_CubeSize;
        m_NodeDatas[id].x = 2 * 5 * m_CubeSize;
        id++;
    }

    for (int i = 0; i < OUTPUT_LAYER; i++)
    {
        m_NodeDatas[id].activation = 0.0;
        m_NodeDatas[id].y = i * 2 * m_CubeSize + 16 * m_CubeSize;
        m_NodeDatas[id].x = 3 * 5 * m_CubeSize;
        id++;
    }
}

void Network::Test()
{
}

void Network::Set()
{
}
