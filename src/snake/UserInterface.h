#pragma once

#include "Render.h"

class SnakeUI : public Render
{
public:
    SnakeUI(GLFWwindow* window, WindowData* data, UIData* uidata, GameData* gamedata);
    ~SnakeUI();

    void OnUpdate(float deltaTime);
    void OnRender();

private:
    std::unique_ptr<IndexBuffer> m_IndexBufferLines;
    float m_SizePixel;
};

class NetworkUI : public Render
{
public:
    NetworkUI(GLFWwindow* window, WindowData* data, UIData* uidata, GameData* gamedata);
    ~NetworkUI();

    void OnUpdate(float deltaTime);
    void OnRender();

private:
    std::unique_ptr<IndexBuffer> m_IndexBufferLines;
    float m_SizePixel;
};

class UserInterface
{
public:
    UserInterface(GLFWwindow* window, WindowData* data, UIData* uidata, GameData* gamedata);
    ~UserInterface();

    void OnUpdate(float deltaTime);
    void OnRender();

private:
    NetworkUI m_NetworkUI;
    SnakeUI m_SnakeUI;

    WindowData* m_Data;
    UIData* m_UIData;
    GameData* m_GameData;

    bool m_DraggingSnake, m_DraggingNetwork;
    float m_SizePixel;
};

// 0.1015f, 0.1054f, 0.1484f, 1.0f