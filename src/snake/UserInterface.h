#pragma once

#include "Render.h"

class UserInterface : public Render
{
public:
    UserInterface(GLFWwindow* window, WindowData* data, UIData* uidata, GameData* gamedata);
    ~UserInterface();

    void OnUpdate(float deltaTime);
    void OnRender();

private:
    std::unique_ptr<IndexBuffer> m_IndexBufferLines;
    bool m_Dragging;
    float m_SizePixel;
};

// 0.1015f, 0.1054f, 0.1484f, 1.0f