#pragma once

#include "Render.h"
#include "Machine.h"
#include "Network.h"

class Snake : public Render
{
public:
    Snake(GLFWwindow* window, WindowData* data, UIData* uidata, GameData* gamedata, Life* bestonedata);
    ~Snake();

    void OnUpdate(float deltaTime);
    void OnRender();

private:
    void Move();
    void ChangeDirection();
    void OtherInputs();
    bool Collision();
    void Erase();
    void Food();
    void Smooth();
    bool Cross();
    int Where(int Thisx, int Thisy, int toThisx, int toThisy, int fallback);
    void ResetSnake();
    bool GameStep();
    void Lost(bool collision = true);
    unsigned int MakeDecision();
    void EvaluateInputs();
    void Save(Life* DataPointer);
    void Load();

    void print();
    void Debug(std::string message);

    Life* m_BestOneData;
    Life m_SnakeData;
    Life m_LoadedSnakeData;

    std::vector<glm::vec4> m_Snake;

    unsigned int m_Direction, m_Prev, m_LifeTime, m_MoveTime, m_FoodCount, m_Steps;
    bool m_Food, m_Start, m_AlreadyCross, m_Machine, m_IsLoaded;
    float m_Accumulator;
    glm::vec2 m_WishHead, m_PrevTail, m_MakeBody;
};