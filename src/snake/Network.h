#pragma once

#include "Render.h"

struct NodeData
{
    float activation;
    float x;
    float y;
};

class Network : public Render
{
public:
    Network(GLFWwindow* window, WindowData* data, UIData* uidata, GameData* gamedata);
    ~Network();

    void OnUpdate(float deltaTime);
    void OnRender();

private:
    void Flush();
    void Set();
    float Normalize(float max, float min, float val);

    Life* m_SnakeData;

    std::vector<glm::vec2> m_NetworkPos;

    static const int m_NodeCount = INPUT_LAYER + FIRST_LAYER + SECOND_LAYER + OUTPUT_LAYER;
    float m_CubeSize;
    
    NodeData m_NodeDatas[m_NodeCount];
};