#include "UserInterface.h"

UserInterface::UserInterface(GLFWwindow *window, WindowData *data, UIData *uidata, GameData *gamedata) : 
    m_SnakeUI(window, data, uidata, gamedata),
    m_NetworkUI(window, data, uidata, gamedata)
{
    m_Data = data;
    m_UIData = uidata;
    m_GameData = gamedata;
    m_DraggingSnake = false;
    m_DraggingNetwork = false;
}

UserInterface::~UserInterface()
{
}

void UserInterface::OnUpdate(float deltaTime)
{
    if (m_Data->pressed)
    {
        if ((m_UIData->TranslateSnake.x + m_GameData->grid_size > m_Data->mouseX &&
             m_Data->mouseX > m_UIData->TranslateSnake.x) &&
            (m_UIData->TranslateSnake.y + (m_SnakeUI.m_SnakeHeight) + (m_GameData->grid_size / 2) > m_Data->mouseY &&
             m_Data->mouseY > m_UIData->TranslateSnake.y + m_SnakeUI.m_SnakeHeight))
        {
            m_DraggingSnake = true;
        }
        
        if ((m_UIData->TranslateNetwork.x + m_GameData->grid_size > m_Data->mouseX &&
             m_Data->mouseX > m_UIData->TranslateNetwork.x) &&
            (m_UIData->TranslateNetwork.y + (m_NetworkUI.m_NetworkHeight) + (m_GameData->grid_size / 2) > m_Data->mouseY &&
             m_Data->mouseY > m_UIData->TranslateNetwork.y + m_NetworkUI.m_NetworkHeight))
        {
            m_DraggingNetwork = true;
        }
    } else
    {
        m_DraggingSnake = false;
        m_DraggingNetwork = false;
    }
    
    if (m_DraggingSnake)
    {
        m_UIData->TranslateSnake = {m_Data->mouseX - (m_GameData->grid_size / 2), m_Data->mouseY - ((m_SnakeUI.m_SnakeHeight) + (m_GameData->grid_size / 4))};
        m_DraggingNetwork = false;
    }

    if (m_DraggingNetwork)
    {
        m_UIData->TranslateNetwork = {m_Data->mouseX - (m_GameData->grid_size / 2), m_Data->mouseY - ((m_NetworkUI.m_NetworkHeight) + (m_GameData->grid_size / 4))};
    }
}

void UserInterface::OnRender()
{
    m_SnakeUI.OnRender();
    m_NetworkUI.OnRender();
}

SnakeUI::SnakeUI(GLFWwindow* window, WindowData* data, UIData* uidata, GameData* gamedata)
{
    m_Window = window;
    m_Data = data;
    m_UIData = uidata;
    m_GameData = gamedata;
    m_Proj = data->projection;
    m_SnakeHeight = gamedata->grid_size * gamedata->dimension;

    float vertex[] = {
    //  Positions
                         0.0f,        0.0f,
                m_SnakeHeight,        0.0f,
                m_SnakeHeight, m_SnakeHeight,
                         0.0f, m_SnakeHeight,
        m_GameData->grid_size, m_SnakeHeight,
        m_GameData->grid_size, m_SnakeHeight + m_GameData->grid_size / 2,
                         0.0f, m_SnakeHeight + m_GameData->grid_size / 2
    };

    unsigned int indicesLine[] = {
        0, 1,
        1, 2,
        2, 3,
        3, 0
    };

    unsigned int indicesTriangle[] = {
        3, 4, 5,
        5, 6, 3
    };

    m_VAO = std::make_unique<VertexArray>();
    m_VertexBuffer = std::make_unique<VertexBuffer>(vertex, sizeof(vertex), GL_STATIC_DRAW);

    VertexBufferLayout vblayout;

    vblayout.Push(GL_FLOAT, 2);

    m_VAO->AddVBuffer(*m_VertexBuffer, vblayout);
    m_IndexBufferLines = std::make_unique<IndexBuffer>(indicesLine, 8);
    m_IndexBuffer = std::make_unique<IndexBuffer>(indicesTriangle, 6);
    m_Shader = std::make_unique<Shader>("../../res/shaders/Interface.shader");
    m_Shader->Bind();
}

SnakeUI::~SnakeUI()
{
}

void SnakeUI::OnUpdate(float deltaTime)
{
}

void SnakeUI::OnRender()
{
    m_Shader->Bind();
    m_Shader->SetUniform2f("u_translateInterface", m_UIData->TranslateSnake.x, m_UIData->TranslateSnake.y);
    m_Shader->SetUniformMat4f("u_proj", m_Data->projection);
    DrawLines(*m_VAO, *m_IndexBufferLines, *m_Shader);
    Draw(*m_VAO, *m_IndexBuffer, *m_Shader);
}

NetworkUI::NetworkUI(GLFWwindow *window, WindowData *data, UIData *uidata, GameData *gamedata)
{
    m_Window = window;
    m_Data = data;
    m_UIData = uidata;
    m_GameData = gamedata;
    m_Proj = data->projection;
    float cube_size = m_GameData->grid_size / 2;
    m_NetworkHeight = 41 * cube_size;

    float vertex[] = {
    //  Positions
                         0.0f, 0.0f,
               18 * cube_size, 0.0f,
               18 * cube_size, m_NetworkHeight,
                         0.0f, m_NetworkHeight,
        m_GameData->grid_size, m_NetworkHeight,
        m_GameData->grid_size, m_NetworkHeight + m_GameData->grid_size / 2,
                         0.0f, m_NetworkHeight + m_GameData->grid_size / 2
    };

    unsigned int indicesLine[] = {
        0, 1,
        1, 2,
        2, 3,
        3, 0
    };

    unsigned int indicesTriangle[] = {
        3, 4, 5,
        5, 6, 3
    };

    m_VAO = std::make_unique<VertexArray>();
    m_VertexBuffer = std::make_unique<VertexBuffer>(vertex, sizeof(vertex), GL_STATIC_DRAW);

    VertexBufferLayout vblayout;

    vblayout.Push(GL_FLOAT, 2);

    m_VAO->AddVBuffer(*m_VertexBuffer, vblayout);
    m_IndexBufferLines = std::make_unique<IndexBuffer>(indicesLine, 8);
    m_IndexBuffer = std::make_unique<IndexBuffer>(indicesTriangle, 6);
    m_Shader = std::make_unique<Shader>("../../res/shaders/Interface.shader");
    m_Shader->Bind();
}

NetworkUI::~NetworkUI()
{
}

void NetworkUI::OnUpdate(float deltaTime)
{
}

void NetworkUI::OnRender()
{
    m_Shader->Bind();
    m_Shader->SetUniform2f("u_translateInterface", m_UIData->TranslateNetwork.x, m_UIData->TranslateNetwork.y);
    m_Shader->SetUniformMat4f("u_proj", m_Data->projection);
    DrawLines(*m_VAO, *m_IndexBufferLines, *m_Shader);
    Draw(*m_VAO, *m_IndexBuffer, *m_Shader);
}