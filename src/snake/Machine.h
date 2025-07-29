#pragma once

#include "Header.h"

#define POPULATION_SIZE 202 // select / 2 * children + elitism
#define LIFETIME 100 // 100
#define LIFEADD 20 // 20
#define SELECT 20
#define ELITISM 2
#define ALPHA 0.4
#define CHILDREN 20
#define MUTATION_RATE 0.1
#define MUTATION 1.0
#define TEST_COUNT 4
#define BOOL_INPUT true

#define REPEAT 20
#define PROGRESS_INTERVAL 20

class Machine
{
public:
    Machine(GameData* gamedata, Life* bestonedata);
    ~Machine();

    void MakeDecision();
    void Shut();
    void Run();

    bool running;
private:
    void Move();
    void ChangeDirection();
    void Collision();
    void Food();
    void Erase();
    void ResetSnake();
    void EvaluateInputs();
    void GameStep();
    void CreatePopulation();
    void TestIndividual(bool display);
    void TestPopulation();
    void SelectPopulation();
    void Crossover();
    void Buffer();
    void WaitGame();
    void WakeUpMachine();

    void DISPLAY();

    Life m_Population[POPULATION_SIZE];
    Life m_PopulationBuffer[POPULATION_SIZE];
    long int m_Fitness[POPULATION_SIZE];

    std::vector<glm::vec2> m_Snake;
    glm::vec2 m_FoodPos;
    bool m_Food, m_Lost, m_Reset;
    unsigned int m_Output, m_Direction, m_Prev, m_ActiveID, m_ID;
    unsigned int m_SelectedPopulation[SELECT];
    GameData* m_GameData;
    Life* m_BestOneData;
};

/*
0 : Sağ Beden
1 : Sol Beden
2 : Üst Beden
3 : Alt Beden
4 : Sağ Yemek
5 : Sol Yemek
6 : Üst Yemek
7 : Alt Yemek
8 : Sağ Üst Yemek
9 : Sol Üst Yemek
10: Sağ Alt Yemek
11: Sol Alt Yemek
12: Sağ Duvar
13: Sol Duvar
14: Üst Duvar
15: Alt Duvar
16: Yılan Yön Sağ
17: Yılan Yön Sol
18: Yılan Yön Yukarı
19: Yılan Yön Aşağı
*/