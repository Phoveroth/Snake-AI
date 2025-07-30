#include "UserInterface.h"

UserInterface::UserInterface(GLFWwindow *window, WindowData *data, UIData *uidata, GameData *gamedata) : 
    m_SnakeUI(window, data, uidata, gamedata),
    m_NetworkUI(window, data, uidata, gamedata),
    m_DraggingSnake(false),
    m_DraggingNetwork(false),
    m_SizePixel(gamedata->grid_size * gamedata->dimension)
{
    m_Data = data;
    m_UIData = uidata;
    m_GameData = gamedata;
}

UserInterface::~UserInterface()
{
}

void UserInterface::OnUpdate(float deltaTime)
{
    if (m_Data->pressed)
    {
        if ((m_UIData->TranslateSnake.x + m_GameData->grid_size > m_Data->mouseX && m_Data->mouseX > m_UIData->TranslateSnake.x) && (m_UIData->TranslateSnake.y + (m_SizePixel) + (m_GameData->grid_size / 2) > m_Data->mouseY && m_Data->mouseY > m_UIData->TranslateSnake.y + m_SizePixel))
        {
            m_DraggingSnake = true;
        }
    } else
    {
        m_DraggingSnake = false;
    }
    
    if (m_DraggingSnake)
    {
        m_UIData->TranslateSnake = {m_Data->mouseX - (m_GameData->grid_size / 2), m_Data->mouseY - ((m_SizePixel) + (m_GameData->grid_size / 4))};
    }
}

void UserInterface::OnRender()
{
}

SnakeUI::SnakeUI(GLFWwindow* window, WindowData* data, UIData* uidata, GameData* gamedata) :
    m_SizePixel(gamedata->grid_size * gamedata->dimension)
{
    m_Window = window;
    m_Data = data;
    m_UIData = uidata;
    m_GameData = gamedata;
    m_Proj = data->projection;
    float cube_size = m_GameData->grid_size / 2;

    float vertex[] = {
    //  Positions
                         0.0f,        0.0f,
                  m_SizePixel,        0.0f,
                  m_SizePixel, m_SizePixel,
                         0.0f, m_SizePixel,
                   -cube_size, -cube_size,
               20 * cube_size, -cube_size,
               20 * cube_size, 40 * cube_size,
                   -cube_size, 40 * cube_size,
        m_GameData->grid_size, m_SizePixel,
        m_GameData->grid_size, m_SizePixel + m_GameData->grid_size / 2,
                         0.0f, m_SizePixel + m_GameData->grid_size / 2
    };

    unsigned int indicesLine[] = {
        0, 1,
        1, 2,
        2, 3,
        3, 0,
        4, 5,
        5, 6,
        6, 7,
        7, 4
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
    m_Shader = std::make_unique<Shader>("../../res/shaders/SnakeInterface.shader");
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
    m_Shader->SetUniform2f("u_translateSnake", m_UIData->TranslateSnake.x, m_UIData->TranslateSnake.y);
    m_Shader->SetUniformMat4f("u_proj", m_Data->projection);
    DrawLines(*m_VAO, *m_IndexBufferLines, *m_Shader);
    Draw(*m_VAO, *m_IndexBuffer, *m_Shader);
}