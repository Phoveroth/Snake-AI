#pragma once

#include "Render.h"

class Network : public Render
{
public:
    Network(GLFWwindow* window, WindowData* data, UIData* uidata, GameData* gamedata, Life* snakedata);
    ~Network();

    void OnUpdate();
    void OnRender();

private:
    void print();
    void Debug(std::string message);

    Life* m_SnakeData;

    std::vector<glm::vec2> m_NetworkPos;
};