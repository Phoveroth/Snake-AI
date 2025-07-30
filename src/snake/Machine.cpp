#include "Machine.h"

Machine::Machine(GameData* gamedata, Life* bestonedata) :
    m_GameData(gamedata),
    m_BestOneData(bestonedata),
    m_Output(GLFW_KEY_UP),
    m_Prev(GLFW_KEY_UP),
    m_Direction(GLFW_KEY_UP),
    m_ActiveID(0),
    m_ID(0),
    m_Food(false),
    m_Lost(false),
    running(true)
{
    ResetSnake();
    CreatePopulation();
}

Machine::~Machine()
{
}

void Machine::MakeDecision()
{
    // Flush The Values
    for (int i = 0; i < FIRST_LAYER; i++)
    {
        m_Population[m_ActiveID].FirstLayer[i].Value = 0;
    }

    for (int i = 0; i < SECOND_LAYER; i++)
    {
        m_Population[m_ActiveID].SecondLayer[i].Value = 0;
    }

    for (int i = 0; i < OUTPUT_LAYER; i++)
    {
        m_Population[m_ActiveID].OutputLayer[i].Value = 0;
    }
    
    // Make The Decision
    for (int i = 0; i < FIRST_LAYER; i++)
    {
        for (int j = 0; j < INPUT_LAYER; j++)
        {
            m_Population[m_ActiveID].FirstLayer[i].Value += m_Population[m_ActiveID].FirstLayer[i].Weights[j] * m_Population[m_ActiveID].InputLayer[j].State;
        }
        m_Population[m_ActiveID].FirstLayer[i].Value += m_Population[m_ActiveID].FirstLayer[i].Bias;

        // RELU
        if (m_Population[m_ActiveID].FirstLayer[i].Value < 0)
        {
            m_Population[m_ActiveID].FirstLayer[i].Value = 0;
        }
    }

    for (int i = 0; i < SECOND_LAYER; i++)
    {
        for (int j = 0; j < FIRST_LAYER; j++)
        {
            m_Population[m_ActiveID].SecondLayer[i].Value += m_Population[m_ActiveID].SecondLayer[i].Weights[j] * m_Population[m_ActiveID].FirstLayer[j].Value;
        }
        m_Population[m_ActiveID].SecondLayer[i].Value += m_Population[m_ActiveID].SecondLayer[i].Bias;

        // RELU
        if (m_Population[m_ActiveID].SecondLayer[i].Value < 0)
        {
            m_Population[m_ActiveID].SecondLayer[i].Value = 0;
        }
    }

    for (int i = 0; i < OUTPUT_LAYER; i++)
    {
        for (int j = 0; j < SECOND_LAYER; j++)
        {
            m_Population[m_ActiveID].OutputLayer[i].Value += m_Population[m_ActiveID].OutputLayer[i].Weights[j] * m_Population[m_ActiveID].SecondLayer[j].Value;
        }
        m_Population[m_ActiveID].OutputLayer[i].Value += m_Population[m_ActiveID].OutputLayer[i].Bias;

        // RELU
        if (m_Population[m_ActiveID].OutputLayer[i].Value < 0)
        {
            m_Population[m_ActiveID].OutputLayer[i].Value = 0;
        }
    }

    float sum = 0;

    for (int i = 0; i < OUTPUT_LAYER; i++)
    {
        sum += m_Population[m_ActiveID].OutputLayer[i].Value;
    }

    float probability = 0;
    float random = static_cast<float>(rand()) / RAND_MAX;

    for (int i = 0; i < OUTPUT_LAYER; i++)
    {
        if (probability <= random && random < probability + (m_Population[m_ActiveID].OutputLayer[i].Value / sum))
        {
            switch (i)
            {
            case 0:
                m_Output = GLFW_KEY_RIGHT;
                break;
            case 1:
                m_Output = GLFW_KEY_LEFT;
                break;
            case 2:
                m_Output = GLFW_KEY_UP;
                break;
            case 3:
                m_Output = GLFW_KEY_DOWN;
                break;
            default:
                Shut();
                break;
            }
            break;
        }
        probability += m_Population[m_ActiveID].OutputLayer[i].Value / sum;
    }
}

void Machine::Shut()
{
    running = false;
}

void Machine::Run()
{   
    const int a = REPEAT * PROGRESS_INTERVAL / 100;
    int count = 0;

    for (int i = 0; i < REPEAT; i++)
    {
        if (i == count)
        {
            std::cout << "\n" << (i * 100) / REPEAT << "%\n";
            count += a;
        }
        TestPopulation();
        SelectPopulation();
        Crossover();
        Buffer();
    }
    
    int max = 0;
    int maxindex = 0;
    for (int i = 0; i < SELECT; i++)
    {
        if (m_Fitness[m_SelectedPopulation[i]] > max)
        {
            max = m_Fitness[i];
            maxindex = i;
        }
    }
    
    *m_BestOneData = m_Population[maxindex];
    m_BestOneData->ID = m_ID;
    m_ID++;

    std::cout << "--------------------\n";
    for (int i = 0; i < SELECT; i++)
    {
        std::cout << m_Fitness[m_SelectedPopulation[i]] << '\n';
    }
    std::cout << "--------------------\n";

    if (m_GameData->machine_print)
    {
        m_GameData->machine_print = false;
        m_ActiveID = maxindex;
        TestIndividual(true);

        std::cout << "\nFIRST LAYER\n";
        for (int i = 0; i < FIRST_LAYER; i++)
        {
            for (int j = 0; j < INPUT_LAYER; j++)
            {
                std::cout << m_Population[m_ActiveID].FirstLayer[i].Weights[j] << '\n';
            }
            std::cout << m_Population[m_ActiveID].FirstLayer[i].Bias << '\n';
        }

        std::cout << "\nSECOND LAYER\n";
        for (int i = 0; i < SECOND_LAYER; i++)
        {
            for (int j = 0; j < FIRST_LAYER; j++)
            {
                std::cout << m_Population[m_ActiveID].SecondLayer[i].Weights[j] << '\n';
            }
            std::cout << m_Population[m_ActiveID].SecondLayer[i].Bias << '\n';
        }
        
        std::cout << "\nOUTPUT LAYER\n";
        for (int i = 0; i < OUTPUT_LAYER; i++)
        {
            for (int j = 0; j < SECOND_LAYER; j++)
            {
                std::cout << m_Population[m_ActiveID].OutputLayer[i].Weights[j] << '\n';
            }
            std::cout << m_Population[m_ActiveID].OutputLayer[i].Bias << '\n';
        }
        
    }
}

void Machine::Move()
{
    glm::vec2 next = m_Snake.back();
    switch (m_Direction)
    {
    case GLFW_KEY_RIGHT: // RIGHT
        next.x++;
        m_Prev = GLFW_KEY_RIGHT;
        break;
    
    case GLFW_KEY_LEFT: // LEFT
        next.x--;
        m_Prev = GLFW_KEY_LEFT;
        break;
    
    case GLFW_KEY_UP: // UP
        next.y++;
        m_Prev = GLFW_KEY_UP;
        break;
    
    case GLFW_KEY_DOWN: // DOWN
        next.y--;
        m_Prev = GLFW_KEY_DOWN;
        break;
    
    default:
        break;
    }

    m_Snake.push_back(next);
    Erase();
}

void Machine::ChangeDirection()
{
    switch (m_Output)
    {
    case GLFW_KEY_RIGHT:
        if (m_Prev != GLFW_KEY_LEFT)
        {
            m_Direction = GLFW_KEY_RIGHT;
        }
        break;
    
    case GLFW_KEY_LEFT:
        if (m_Prev != GLFW_KEY_RIGHT)
        {
            m_Direction = GLFW_KEY_LEFT;
        }
        break;
    
    case GLFW_KEY_UP:
        if (m_Prev != GLFW_KEY_DOWN)
        {
            m_Direction = GLFW_KEY_UP;
        }
        break;
    
    case GLFW_KEY_DOWN:
        if (m_Prev != GLFW_KEY_UP)
        {
            m_Direction = GLFW_KEY_DOWN;
        }
        break;

    default:
        break;
    }
}

void Machine::Collision()
{
    // Food
    if (m_Snake.back() == m_FoodPos)
    {
        Food();
    }
    
    // Body
    for (int i = 0; i < m_Snake.size() - 2; i++)
    {
        if (m_Snake.back() == m_Snake[i])
        {
            m_Lost = true;
        }
    }

    // Border
    if (m_Snake.back().x == -1 || m_Snake.back().x == m_GameData->grid_width)
    {
        m_Lost = true;
    }

    if (m_Snake.back().y == -1 || m_Snake.back().y == m_GameData->grid_height)
    {
        m_Lost = true;
    }
}

void Machine::Food()
{
    bool ok = true;

    while (ok)
    {
        m_FoodPos = {rand() % m_GameData->grid_width, rand() % m_GameData->grid_height};
        ok = false;

        for (int i = 0; i < m_Snake.size(); i++)
        {
            if (m_Snake[i] == m_FoodPos)
            {
                ok = true;
                break;
            }
        }
    }

    m_Food = true;
}

void Machine::Erase()
{
    if (!m_Food)
    {
        m_Snake.erase(m_Snake.begin());
    }
    m_Food = false;
}

void Machine::ResetSnake()
{
    int HeadPosX = m_GameData->grid_width / 2 + 1;
    int HeadPosY = m_GameData->grid_height / 2 + 1;

    m_Snake.clear();

    m_Snake.push_back({HeadPosX, HeadPosY - 3});    // Tail
    m_Snake.push_back({HeadPosX, HeadPosY - 2});    // Body
    m_Snake.push_back({HeadPosX, HeadPosY - 1});    // Body
    m_Snake.push_back({HeadPosX, HeadPosY});        // Head

    m_Direction = GLFW_KEY_UP;
    m_Prev = GLFW_KEY_UP;
    m_Output = GLFW_KEY_UP;

    Food();
    m_Food = false;
}

void Machine::EvaluateInputs()
{
    if (BOOL_INPUT)
    {
        // Beden
        m_Population[m_ActiveID].InputLayer[0].State = 0;
        m_Population[m_ActiveID].InputLayer[1].State = 0;
        m_Population[m_ActiveID].InputLayer[2].State = 0;
        m_Population[m_ActiveID].InputLayer[3].State = 0;
        for (int i = 0; i < m_Snake.size() - 1; i++)
        {
            // Direkt sağında bedeni var mı?
            if (m_Snake.back().x + 1 == m_Snake[i].x && m_Snake.back().y == m_Snake[i].y)
            {
                m_Population[m_ActiveID].InputLayer[0].State = 1;
            }

            // Direkt solunda bedeni var mı?
            if (m_Snake.back().x - 1 == m_Snake[i].x && m_Snake.back().y == m_Snake[i].y)
            {
                m_Population[m_ActiveID].InputLayer[1].State = 1;
            }

            // Direkt üstünde bedeni var mı?
            if (m_Snake.back().x == m_Snake[i].x && m_Snake.back().y + 1 == m_Snake[i].y)
            {
                m_Population[m_ActiveID].InputLayer[2].State = 1;
            }

            // Direkt altında bedeni var mı?
            if (m_Snake.back().x == m_Snake[i].x && m_Snake.back().y - 1 == m_Snake[i].y)
            {
                m_Population[m_ActiveID].InputLayer[3].State = 1;
            }
        }

        // Sağda yem var mı?
        if (m_Snake.back().x < m_FoodPos.x && m_Snake.back().y == m_FoodPos.y)
        {
            m_Population[m_ActiveID].InputLayer[4].State = 1;
        } else { m_Population[m_ActiveID].InputLayer[4].State = 0; }

        // Solda yem var mı?
        if (m_Snake.back().x > m_FoodPos.x && m_Snake.back().y == m_FoodPos.y)
        {
            m_Population[m_ActiveID].InputLayer[5].State = 1;
        } else { m_Population[m_ActiveID].InputLayer[5].State = 0; }

        // Üstte yem var mı?
        if (m_Snake.back().x == m_FoodPos.x && m_Snake.back().y < m_FoodPos.y)
        {
            m_Population[m_ActiveID].InputLayer[6].State = 1;
        } else { m_Population[m_ActiveID].InputLayer[6].State = 0; }

        // Altta yem var mı?
        if (m_Snake.back().x == m_FoodPos.x && m_Snake.back().y > m_FoodPos.y)
        {
            m_Population[m_ActiveID].InputLayer[7].State = 1;
        } else { m_Population[m_ActiveID].InputLayer[7].State = 0; }

        int x_difference = m_FoodPos.x - m_Snake.back().x;
        int y_difference = m_FoodPos.y - m_Snake.back().y;

        // Sağ üstte yem var mı?
        if ((x_difference == y_difference) && (x_difference >= 0))
        {
            m_Population[m_ActiveID].InputLayer[8].State = 1;
        } else { m_Population[m_ActiveID].InputLayer[8].State = 0; }

        // Sol üstte yem var mı?
        if ((- x_difference == y_difference) && (y_difference >= 0))
        {
            m_Population[m_ActiveID].InputLayer[9].State = 1;
        } else { m_Population[m_ActiveID].InputLayer[9].State = 0; }

        // Sağ altta yem var mı?
        if ((x_difference == - y_difference) && (y_difference <= 0))
        {
            m_Population[m_ActiveID].InputLayer[10].State = 1;
        } else { m_Population[m_ActiveID].InputLayer[10].State = 0; }

        // Sol altta yem var mı?
        if ((x_difference == y_difference) && (x_difference <= 0))
        {
            m_Population[m_ActiveID].InputLayer[11].State = 1;
        } else { m_Population[m_ActiveID].InputLayer[11].State = 0; }

        // Direkt sağında duvar var mı?
        if (m_Snake.back().x + 1 == m_GameData->grid_width)
        {
            m_Population[m_ActiveID].InputLayer[12].State = 1;
        } else { m_Population[m_ActiveID].InputLayer[12].State = 0; }

        // Direkt solunda duvar var mı?
        if (m_Snake.back().x - 1 == -1)
        {
            m_Population[m_ActiveID].InputLayer[13].State = 1;
        } else { m_Population[m_ActiveID].InputLayer[13].State = 0; }

        // Direkt üstünde duvar var mı?
        if (m_Snake.back().y + 1 == m_GameData->grid_height)
        {
            m_Population[m_ActiveID].InputLayer[14].State = 1;
        } else { m_Population[m_ActiveID].InputLayer[14].State = 0; }

        // Direkt altında duvar var mı?
        if (m_Snake.back().y - 1 == -1)
        {
            m_Population[m_ActiveID].InputLayer[15].State = 1;
        } else { m_Population[m_ActiveID].InputLayer[15].State = 0; }

        // Yılanın yönü sağ mı?
        if (m_Direction == GLFW_KEY_RIGHT)
        {
            m_Population[m_ActiveID].InputLayer[16].State = 1;
        } else { m_Population[m_ActiveID].InputLayer[16].State = 0; }

        // Yılanın yönü sol mu?
        if (m_Direction == GLFW_KEY_LEFT)
        {
            m_Population[m_ActiveID].InputLayer[17].State = 1;
        } else { m_Population[m_ActiveID].InputLayer[17].State = 0; }

        // Yılanın yönü yukarı mı?
        if (m_Direction == GLFW_KEY_UP)
        {
            m_Population[m_ActiveID].InputLayer[18].State = 1;
        } else { m_Population[m_ActiveID].InputLayer[18].State = 0; }

        // Yılanın yönü aşağı mı?
        if (m_Direction == GLFW_KEY_DOWN)
        {
            m_Population[m_ActiveID].InputLayer[19].State = 1;
        } else { m_Population[m_ActiveID].InputLayer[19].State = 0; }

    } else
    {
        // Sağ Beden
        int look = m_Snake.back().x;
        bool stop = false;
        while (look < m_GameData->grid_width)
        {
            look++;
            for (int i = 1; i < m_Snake.size(); i++)
            {
                if (look == m_Snake.at(i).x && m_Snake.back().y == m_Snake.at(i).y)
                {
                    m_Population[m_ActiveID].InputLayer[0].State = 1 - ((float)(look - (m_Snake.back().x + 1)) / m_GameData->grid_width);
                    stop = true;
                    break;
                }
                m_Population[m_ActiveID].InputLayer[0].State = 0;
            }
            if (stop) break;
        }

        // Sol Beden
        look = m_Snake.back().x;
        stop = false;
        while (look > -1)
        {
            look--;
            for (int i = 1; i < m_Snake.size(); i++)
            {
                if (look == m_Snake.at(i).x && m_Snake.back().y == m_Snake.at(i).y)
                {
                    m_Population[m_ActiveID].InputLayer[1].State = 1 - ((float)((m_Snake.back().x - 1) - look) / m_GameData->grid_width);
                    stop = true;
                    break;
                }
                m_Population[m_ActiveID].InputLayer[1].State = 0;
            }
            if (stop) break;
        }

        // Üst Beden
        look = m_Snake.back().y;
        stop = false;
        while (look < m_GameData->grid_height)
        {
            look++;
            for (int i = 1; i < m_Snake.size(); i++)
            {
                if (look == m_Snake.at(i).y && m_Snake.back().x == m_Snake.at(i).x)
                {
                    m_Population[m_ActiveID].InputLayer[2].State = 1 - ((float)(look - (m_Snake.back().y + 1)) / m_GameData->grid_width);
                    stop = true;
                    break;
                }
                m_Population[m_ActiveID].InputLayer[2].State = 0;
            }
            if (stop) break;
        }

        // Alt Beden
        look = m_Snake.back().y;
        stop = false;
        while (look > -1)
        {
            look--;
            for (int i = 1; i < m_Snake.size(); i++)
            {
                if (look == m_Snake.at(i).y && m_Snake.back().x == m_Snake.at(i).x)
                {
                    m_Population[m_ActiveID].InputLayer[3].State = 1 - ((float)((m_Snake.back().y - 1) - look) / m_GameData->grid_width);
                    stop = true;
                    break;
                }
                m_Population[m_ActiveID].InputLayer[3].State = 0;
            }
            if (stop) break;
        }

        // Sağda yem var mı?
        if (m_Snake.back().x < m_FoodPos.x && m_Snake.back().y == m_FoodPos.y)
        {
            m_Population[m_ActiveID].InputLayer[4].State = 1 - ((float)(m_FoodPos.x - (m_Snake.back().x + 1)) / m_GameData->grid_width);
        } else { m_Population[m_ActiveID].InputLayer[4].State = 0; }

        // Solda yem var mı?
        if (m_Snake.back().x > m_FoodPos.x && m_Snake.back().y == m_FoodPos.y)
        {
            m_Population[m_ActiveID].InputLayer[5].State = 1 - ((float)((m_Snake.back().x - 1) - m_FoodPos.x) / m_GameData->grid_width);
        } else { m_Population[m_ActiveID].InputLayer[5].State = 0; }

        // Üstte yem var mı?
        if (m_Snake.back().x == m_FoodPos.x && m_Snake.back().y < m_FoodPos.y)
        {
            m_Population[m_ActiveID].InputLayer[6].State = 1 - ((float)(m_FoodPos.y - (m_Snake.back().y + 1)) / m_GameData->grid_height);
        } else { m_Population[m_ActiveID].InputLayer[6].State = 0; }

        // Altta yem var mı?
        if (m_Snake.back().x == m_FoodPos.x && m_Snake.back().y > m_FoodPos.y)
        {
            m_Population[m_ActiveID].InputLayer[7].State = 1 - ((float)((m_Snake.back().y - 1) - m_FoodPos.y) / m_GameData->grid_height);
        } else { m_Population[m_ActiveID].InputLayer[7].State = 0; }

        int x_difference = m_FoodPos.x - m_Snake.back().x;
        int y_difference = m_FoodPos.y - m_Snake.back().y;

        // Sağ üstte yem var mı?
        if ((x_difference == y_difference) && (x_difference >= 0))
        {
            m_Population[m_ActiveID].InputLayer[8].State = 1 - ((float)(x_difference + y_difference - 2) / (m_GameData->grid_width + m_GameData->grid_height));
        } else { m_Population[m_ActiveID].InputLayer[8].State = 0; }

        // Sol üstte yem var mı?
        if ((- x_difference == y_difference) && (y_difference >= 0))
        {
            m_Population[m_ActiveID].InputLayer[9].State = 1 - ((float)(- x_difference + y_difference - 2) / (m_GameData->grid_width + m_GameData->grid_height));
        } else { m_Population[m_ActiveID].InputLayer[9].State = 0; }

        // Sağ altta yem var mı?
        if ((x_difference == - y_difference) && (y_difference <= 0))
        {
            m_Population[m_ActiveID].InputLayer[10].State = 1 - ((float)(x_difference - y_difference - 2) / (m_GameData->grid_width + m_GameData->grid_height));
        } else { m_Population[m_ActiveID].InputLayer[10].State = 0; }

        // Sol altta yem var mı?
        if ((x_difference == y_difference) && (x_difference <= 0))
        {
            m_Population[m_ActiveID].InputLayer[11].State = 1 - ((float)(- x_difference - y_difference - 2) / (m_GameData->grid_width + m_GameData->grid_height));
        } else { m_Population[m_ActiveID].InputLayer[11].State = 0; }

        // Sağ Duvar
        m_Population[m_ActiveID].InputLayer[12].State = 1 - ((float)(m_GameData->grid_width - (m_Snake.back().x + 1)) / m_GameData->grid_width);

        // Sol Duvar
        m_Population[m_ActiveID].InputLayer[13].State = 1 - ((float)(m_Snake.back().x) / m_GameData->grid_width);

        // Üst Duvar
        m_Population[m_ActiveID].InputLayer[14].State = 1 - ((float)(m_GameData->grid_height - (m_Snake.back().y + 1)) / m_GameData->grid_height);

        // Alt Duvar
        m_Population[m_ActiveID].InputLayer[15].State = 1 - ((float)(m_Snake.back().y) / m_GameData->grid_height);
        
        // Yılanın yönü sağ mı?
        if (m_Direction == GLFW_KEY_RIGHT)
        {
            m_Population[m_ActiveID].InputLayer[16].State = 1;
        } else { m_Population[m_ActiveID].InputLayer[16].State = 0; }

        // Yılanın yönü sol mu?
        if (m_Direction == GLFW_KEY_LEFT)
        {
            m_Population[m_ActiveID].InputLayer[17].State = 1;
        } else { m_Population[m_ActiveID].InputLayer[17].State = 0; }

        // Yılanın yönü yukarı mı?
        if (m_Direction == GLFW_KEY_UP)
        {
            m_Population[m_ActiveID].InputLayer[18].State = 1;
        } else { m_Population[m_ActiveID].InputLayer[18].State = 0; }

        // Yılanın yönü aşağı mı?
        if (m_Direction == GLFW_KEY_DOWN)
        {
            m_Population[m_ActiveID].InputLayer[19].State = 1;
        } else { m_Population[m_ActiveID].InputLayer[19].State = 0; }
        
    }
}

void Machine::GameStep()
{
    EvaluateInputs();
    MakeDecision();
    ChangeDirection();
    Move();
    Collision();
}

void Machine::CreatePopulation()
{
    for (int a = 0; a < 100; a++)
    {
        for (int i = 0; i < FIRST_LAYER; i++)
        {
            for (int j = 0; j < INPUT_LAYER; j++)
            {
                m_Population[a].FirstLayer[i].Weights[j] = rand() / (float)RAND_MAX;
            }
            m_Population[a].FirstLayer[i].Bias = 2 * (rand() / (float)RAND_MAX) - 1;
        }

        for (int i = 0; i < SECOND_LAYER; i++)
        {
            for (int j = 0; j < FIRST_LAYER; j++)
            {
                m_Population[a].SecondLayer[i].Weights[j] = rand() / (float)RAND_MAX;
            }
            m_Population[a].SecondLayer[i].Bias = 2 * (rand() / (float)RAND_MAX) - 1;
        }

        for (int i = 0; i < OUTPUT_LAYER; i++)
        {
            for (int j = 0; j < SECOND_LAYER; j++)
            {
                m_Population[a].OutputLayer[i].Weights[j] = rand() / (float)RAND_MAX;
            }
            m_Population[a].OutputLayer[i].Bias = 2 * (rand() / (float)RAND_MAX) - 1;
        }
    }
    
}

void Machine::TestIndividual(bool display)
{
    ResetSnake();

    unsigned int steps = 0;

    for (int i = LIFETIME; i >= 0; i--)
    {
        GameStep();

        steps++;

        if (m_Food)
        {
            i += LIFEADD;
        }

        if (m_Lost || i == 0)
        {
            unsigned int Food = m_Snake.size() - 4;

            if (!display)
            {
                m_Fitness[m_ActiveID] += Fitness_Function(Food, steps, m_Lost); // Max 300,000
            } else std::cout << "\nMACHINE SCORE: " << Food << '\n';

            m_Lost = false;
            break;
        }

        if (display)
            DISPLAY();
    }
    
}

void Machine::TestPopulation()
{
    m_ActiveID = 0;

    for (int i = 0; i < POPULATION_SIZE; i++)
    {
        m_Fitness[m_ActiveID] = 0;

        for (int i = 0; i < TEST_COUNT; i++)
        {
            TestIndividual(false);
        }

        m_Fitness[m_ActiveID] /= TEST_COUNT;

        m_ActiveID++;
    }
}

void Machine::SelectPopulation()
{
    for (int i = 0; i < SELECT; i++)
    {
        m_SelectedPopulation[i] = i;
    }
    
    for (int i = SELECT; i < POPULATION_SIZE; i++)
    {
        for (int j = 0; j < SELECT; j++)
        {
            if (m_Fitness[i] > m_Fitness[m_SelectedPopulation[j]])
            {
                int temp = m_SelectedPopulation[j];
                m_SelectedPopulation[j] = i;

                while (j < SELECT - 1)
                {   
                    j++;
                    if (m_Fitness[temp] > m_Fitness[m_SelectedPopulation[j]])
                    {
                        int tempp = m_SelectedPopulation[j];
                        m_SelectedPopulation[j] = temp;
                        temp = tempp;
                    }
                }
            }
        }
    }

    // Bubble Sort
    int temp;
    for (int i = 0; i < SELECT; i++)
    {
        for (int j = i + 1; j < SELECT; j++)
        {
            if (m_Fitness[m_SelectedPopulation[i]] < m_Fitness[m_SelectedPopulation[j]])
            {
                temp = m_SelectedPopulation[i];
                m_SelectedPopulation[i] = m_SelectedPopulation[j];
                m_SelectedPopulation[j] = temp;
            }
        }
    }
    
}

void Machine::Crossover()
{
    int current = 0;

    for (int i = 0; i < ELITISM; i++)
    {
        m_PopulationBuffer[i] = m_Population[m_SelectedPopulation[i]];
        current++;
    }

    for (int i = 0; i < SELECT; i += 2)
    {
        Life Children[CHILDREN];

        for (int f = 0; f < FIRST_LAYER; f++)
        {
            float lower, upper;

            for (int n = 0; n < INPUT_LAYER; n++)
            {
                if (m_Population[m_SelectedPopulation[i]].FirstLayer[f].Weights[n] > m_Population[m_SelectedPopulation[i + 1]].FirstLayer[f].Weights[n])
                {
                    lower = m_Population[m_SelectedPopulation[i + 1]].FirstLayer[f].Weights[n] - ALPHA * (m_Population[m_SelectedPopulation[i]].FirstLayer[f].Weights[n] - m_Population[m_SelectedPopulation[i + 1]].FirstLayer[f].Weights[n]);
                    upper = m_Population[m_SelectedPopulation[i]].FirstLayer[f].Weights[n] + ALPHA * (m_Population[m_SelectedPopulation[i]].FirstLayer[f].Weights[n] - m_Population[m_SelectedPopulation[i + 1]].FirstLayer[f].Weights[n]);
                } else
                {
                    lower = m_Population[m_SelectedPopulation[i]].FirstLayer[f].Weights[n] - ALPHA * (m_Population[m_SelectedPopulation[i + 1]].FirstLayer[f].Weights[n] - m_Population[m_SelectedPopulation[i]].FirstLayer[f].Weights[n]);
                    upper = m_Population[m_SelectedPopulation[i + 1]].FirstLayer[f].Weights[n] + ALPHA * (m_Population[m_SelectedPopulation[i + 1]].FirstLayer[f].Weights[n] - m_Population[m_SelectedPopulation[i]].FirstLayer[f].Weights[n]);
                }
                
                for (int c = 0; c < CHILDREN; c++)
                {
                    Children[c].FirstLayer[f].Weights[n] = lower + (rand() / (float)RAND_MAX) * (upper - lower);

                    // Mutate
                    if ((rand() / (float)RAND_MAX) < MUTATION_RATE)
                    {
                        Children[c].FirstLayer[f].Weights[n] += (rand() / (float)RAND_MAX) * 2 * MUTATION - MUTATION;
                    }
                }
            }

            if (m_Population[m_SelectedPopulation[i]].FirstLayer[f].Bias > m_Population[m_SelectedPopulation[i + 1]].FirstLayer[f].Bias)
            {
                lower = m_Population[m_SelectedPopulation[i + 1]].FirstLayer[f].Bias - ALPHA * (m_Population[m_SelectedPopulation[i]].FirstLayer[f].Bias - m_Population[m_SelectedPopulation[i + 1]].FirstLayer[f].Bias);
                upper = m_Population[m_SelectedPopulation[i]].FirstLayer[f].Bias + ALPHA * (m_Population[m_SelectedPopulation[i]].FirstLayer[f].Bias - m_Population[m_SelectedPopulation[i + 1]].FirstLayer[f].Bias);
            } else
            {
                lower = m_Population[m_SelectedPopulation[i]].FirstLayer[f].Bias - ALPHA * (m_Population[m_SelectedPopulation[i + 1]].FirstLayer[f].Bias - m_Population[m_SelectedPopulation[i]].FirstLayer[f].Bias);
                upper = m_Population[m_SelectedPopulation[i + 1]].FirstLayer[f].Bias + ALPHA * (m_Population[m_SelectedPopulation[i + 1]].FirstLayer[f].Bias - m_Population[m_SelectedPopulation[i]].FirstLayer[f].Bias);
            }

            for (int c = 0; c < CHILDREN; c++)
            {
                Children[c].FirstLayer[f].Bias = lower + (rand() / (float)RAND_MAX) * (upper - lower);

                // Mutate
                if ((rand() / (float)RAND_MAX) < MUTATION_RATE)
                {
                    Children[c].FirstLayer[f].Bias += (rand() / (float)RAND_MAX) * 2 * MUTATION - MUTATION;
                }
            }
        }

        for (int s = 0; s < SECOND_LAYER; s++)
        {
            float lower, upper;

            for (int f = 0; f < FIRST_LAYER; f++)
            {
                if (m_Population[m_SelectedPopulation[i]].SecondLayer[s].Weights[f] > m_Population[m_SelectedPopulation[i + 1]].SecondLayer[s].Weights[f])
                {
                    lower = m_Population[m_SelectedPopulation[i + 1]].SecondLayer[s].Weights[f] - ALPHA * (m_Population[m_SelectedPopulation[i]].SecondLayer[s].Weights[f] - m_Population[m_SelectedPopulation[i + 1]].SecondLayer[s].Weights[f]);
                    upper = m_Population[m_SelectedPopulation[i]].SecondLayer[s].Weights[f] + ALPHA * (m_Population[m_SelectedPopulation[i]].SecondLayer[s].Weights[f] - m_Population[m_SelectedPopulation[i + 1]].SecondLayer[s].Weights[f]);
                } else
                {
                    lower = m_Population[m_SelectedPopulation[i]].SecondLayer[s].Weights[f] - ALPHA * (m_Population[m_SelectedPopulation[i + 1]].SecondLayer[s].Weights[f] - m_Population[m_SelectedPopulation[i]].SecondLayer[s].Weights[f]);
                    upper = m_Population[m_SelectedPopulation[i + 1]].SecondLayer[s].Weights[f] + ALPHA * (m_Population[m_SelectedPopulation[i + 1]].SecondLayer[s].Weights[f] - m_Population[m_SelectedPopulation[i]].SecondLayer[s].Weights[f]);
                }
                
                for (int c = 0; c < CHILDREN; c++)
                {
                    Children[c].SecondLayer[s].Weights[f] = lower + (rand() / (float)RAND_MAX) * (upper - lower);

                    // Mutate
                    if ((rand() / (float)RAND_MAX) < MUTATION_RATE)
                    {
                        Children[c].SecondLayer[s].Weights[f] += (rand() / (float)RAND_MAX) * 2 * MUTATION - MUTATION;
                    }
                }
            }

            if (m_Population[m_SelectedPopulation[i]].SecondLayer[s].Bias > m_Population[m_SelectedPopulation[i + 1]].SecondLayer[s].Bias)
            {
                lower = m_Population[m_SelectedPopulation[i + 1]].SecondLayer[s].Bias - ALPHA * (m_Population[m_SelectedPopulation[i]].SecondLayer[s].Bias - m_Population[m_SelectedPopulation[i + 1]].SecondLayer[s].Bias);
                upper = m_Population[m_SelectedPopulation[i]].SecondLayer[s].Bias + ALPHA * (m_Population[m_SelectedPopulation[i]].SecondLayer[s].Bias - m_Population[m_SelectedPopulation[i + 1]].SecondLayer[s].Bias);
            } else
            {
                lower = m_Population[m_SelectedPopulation[i]].SecondLayer[s].Bias - ALPHA * (m_Population[m_SelectedPopulation[i + 1]].SecondLayer[s].Bias - m_Population[m_SelectedPopulation[i]].SecondLayer[s].Bias);
                upper = m_Population[m_SelectedPopulation[i + 1]].SecondLayer[s].Bias + ALPHA * (m_Population[m_SelectedPopulation[i + 1]].SecondLayer[s].Bias - m_Population[m_SelectedPopulation[i]].SecondLayer[s].Bias);
            }

            for (int c = 0; c < CHILDREN; c++)
            {
                Children[c].SecondLayer[s].Bias = lower + (rand() / (float)RAND_MAX) * (upper - lower);

                // Mutate
                if ((rand() / (float)RAND_MAX) < MUTATION_RATE)
                {
                    Children[c].SecondLayer[s].Bias += (rand() / (float)RAND_MAX) * 2 * MUTATION - MUTATION;
                }
            }
        }

        for (int o = 0; o < OUTPUT_LAYER; o++)
        {
            float lower, upper;

            for (int s = 0; s < SECOND_LAYER; s++)
            {
                if (m_Population[m_SelectedPopulation[i]].OutputLayer[o].Weights[s] > m_Population[m_SelectedPopulation[i + 1]].OutputLayer[o].Weights[s])
                {
                    lower = m_Population[m_SelectedPopulation[i + 1]].OutputLayer[o].Weights[s] - ALPHA * (m_Population[m_SelectedPopulation[i]].OutputLayer[o].Weights[s] - m_Population[m_SelectedPopulation[i + 1]].OutputLayer[o].Weights[s]);
                    upper = m_Population[m_SelectedPopulation[i]].OutputLayer[o].Weights[s] + ALPHA * (m_Population[m_SelectedPopulation[i]].OutputLayer[o].Weights[s] - m_Population[m_SelectedPopulation[i + 1]].OutputLayer[o].Weights[s]);
                } else
                {
                    lower = m_Population[m_SelectedPopulation[i]].OutputLayer[o].Weights[s] - ALPHA * (m_Population[m_SelectedPopulation[i + 1]].OutputLayer[o].Weights[s] - m_Population[m_SelectedPopulation[i]].OutputLayer[o].Weights[s]);
                    upper = m_Population[m_SelectedPopulation[i + 1]].OutputLayer[o].Weights[s] + ALPHA * (m_Population[m_SelectedPopulation[i + 1]].OutputLayer[o].Weights[s] - m_Population[m_SelectedPopulation[i]].OutputLayer[o].Weights[s]);
                }
                
                for (int c = 0; c < CHILDREN; c++)
                {
                    Children[c].OutputLayer[o].Weights[s] = lower + (rand() / (float)RAND_MAX) * (upper - lower);

                    // Mutate
                    if ((rand() / (float)RAND_MAX) < MUTATION_RATE)
                    {
                        Children[c].OutputLayer[o].Weights[s] += (rand() / (float)RAND_MAX) * 2 * MUTATION - MUTATION;
                    }
                }
            }

            if (m_Population[m_SelectedPopulation[i]].OutputLayer[o].Bias > m_Population[m_SelectedPopulation[i + 1]].OutputLayer[o].Bias)
            {
                lower = m_Population[m_SelectedPopulation[i + 1]].OutputLayer[o].Bias - ALPHA * (m_Population[m_SelectedPopulation[i]].OutputLayer[o].Bias - m_Population[m_SelectedPopulation[i + 1]].OutputLayer[o].Bias);
                upper = m_Population[m_SelectedPopulation[i]].OutputLayer[o].Bias + ALPHA * (m_Population[m_SelectedPopulation[i]].OutputLayer[o].Bias - m_Population[m_SelectedPopulation[i + 1]].OutputLayer[o].Bias);
            } else
            {
                lower = m_Population[m_SelectedPopulation[i]].OutputLayer[o].Bias - ALPHA * (m_Population[m_SelectedPopulation[i + 1]].OutputLayer[o].Bias - m_Population[m_SelectedPopulation[i]].OutputLayer[o].Bias);
                upper = m_Population[m_SelectedPopulation[i + 1]].OutputLayer[o].Bias + ALPHA * (m_Population[m_SelectedPopulation[i + 1]].OutputLayer[o].Bias - m_Population[m_SelectedPopulation[i]].OutputLayer[o].Bias);
            }

            for (int c = 0; c < CHILDREN; c++)
            {
                Children[c].OutputLayer[o].Bias = lower + (rand() / (float)RAND_MAX) * (upper - lower);

                // Mutate
                if ((rand() / (float)RAND_MAX) < MUTATION_RATE)
                {
                    Children[c].OutputLayer[o].Bias += (rand() / (float)RAND_MAX) * 2 * MUTATION - MUTATION;
                }
            }
        }

        for (int p = 0; p < CHILDREN; p++)
        {
            m_PopulationBuffer[current] = Children[p];
            current++;
        }
    }
    
    
}

void Machine::Buffer()
{
    for (int i = 0; i < POPULATION_SIZE; i++)
    {
        m_Population[i] = m_PopulationBuffer[i];
    }
}

void Machine::DISPLAY()
{
    for (int y = m_GameData->grid_height - 1; y >= 0; y--)
    {
        for (int x = 0; x < m_GameData->grid_width; x++)
        {
            glm::vec2 a = {x, y};
            int b = 0;

            if (a == m_FoodPos)
            {
                b = 1;
            }
            
            for (int i = 0; i < m_Snake.size(); i++)
            {
                if (a == m_Snake[i])
                {
                    b = 2;
                    break;
                }
                
            }

            switch (b)
            {
            case 1:
                std::cout << "A ";
                break;
            case 2:
                std::cout << "O ";
                break;
            default:
                std::cout << ". ";
                break;
            }
            
            
        }
        std::cout << '\n';
    }

    EvaluateInputs();
    std::cout << "---\n";
    std::cout << m_Population[m_ActiveID].InputLayer[0].State <<  " Sag Beden\n";
    std::cout << m_Population[m_ActiveID].InputLayer[1].State <<  " Sol Beden\n";
    std::cout << m_Population[m_ActiveID].InputLayer[2].State <<  " Ust Beden\n";
    std::cout << m_Population[m_ActiveID].InputLayer[3].State <<  " Alt Beden\n";
    std::cout << "---\n";
    std::cout << m_Population[m_ActiveID].InputLayer[4].State <<  " Sag Yem\n";
    std::cout << m_Population[m_ActiveID].InputLayer[5].State <<  " Sol Yem\n";
    std::cout << m_Population[m_ActiveID].InputLayer[6].State <<  " Ust Yem\n";
    std::cout << m_Population[m_ActiveID].InputLayer[7].State <<  " Alt Yem\n";
    std::cout << m_Population[m_ActiveID].InputLayer[8].State <<  " Sag Ust Yem\n";
    std::cout << m_Population[m_ActiveID].InputLayer[9].State <<  " Sol Ust Yem\n";
    std::cout << m_Population[m_ActiveID].InputLayer[10].State << " Sag Alt Yem\n";
    std::cout << m_Population[m_ActiveID].InputLayer[11].State << " Sol Alt Yem\n";
    std::cout << "---\n";
    std::cout << m_Population[m_ActiveID].InputLayer[12].State << " Sag Duvar\n";
    std::cout << m_Population[m_ActiveID].InputLayer[13].State << " Sol Duvar\n";
    std::cout << m_Population[m_ActiveID].InputLayer[14].State << " Ust Duvar\n";
    std::cout << m_Population[m_ActiveID].InputLayer[15].State << " Alt Duvar\n";
    std::cout << "---\n";
    std::cout << m_Population[m_ActiveID].InputLayer[16].State << " Yon Sag Mi?\n";
    std::cout << m_Population[m_ActiveID].InputLayer[17].State << " Yon Sol Mi?\n";
    std::cout << m_Population[m_ActiveID].InputLayer[18].State << " Yon Ust Mi?\n";
    std::cout << m_Population[m_ActiveID].InputLayer[19].State << " Yon Alt Mi?\n";

    std::cin.get();
}

/*
RIGHT: 0
LEFT:  1
UP:    2
DOWN:  3
*/