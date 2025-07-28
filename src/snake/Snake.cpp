#include "Snake.h"

Snake::Snake(GLFWwindow* window, WindowData* data, UIData* uidata, GameData* gamedata, Life* bestonedata) :
    m_MakeBody(BODY, 1),
    m_Prev(GLFW_KEY_UP),
    m_Machine(true),
    m_Start(false),
    m_LifeTime(LIFETIME),
    m_MoveTime(60),
    m_BestOneData(bestonedata)
{
    m_Window = window;
    m_Data = data;
    m_UIData = uidata;
    m_GameData = gamedata;
    m_Proj = data ->projection;

    float vertex[] = {
    //  Positions                                     TexCoords
                         0.0f,                  0.0f,                   0.0f,                   0.0f, // Bottom Left
        m_GameData->grid_size,                  0.0f, m_GameData->atlas_size,                   0.0f, // Bottom Right
        m_GameData->grid_size, m_GameData->grid_size, m_GameData->atlas_size, m_GameData->atlas_size, // Top Right
                         0.0f, m_GameData->grid_size,                   0.0f, m_GameData->atlas_size  // Top Left
    };

    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    ResetSnake();

    m_VAO = std::make_unique<VertexArray>();
    m_VertexBuffer = std::make_unique<VertexBuffer>(vertex, sizeof(vertex), GL_STATIC_DRAW);
    m_InstanceBuffer = std::make_unique<InstanceBuffer>();
    m_InstanceBuffer->Data(m_Snake.data(), m_Snake.size() * sizeof(glm::vec4), GL_DYNAMIC_DRAW);

    VertexBufferLayout vblayout;
    VertexBufferLayout ivblayout;

    vblayout.Push(GL_FLOAT, 2);
    vblayout.Push(GL_FLOAT, 2);
    ivblayout.Push(GL_FLOAT, 2);
    ivblayout.Push(GL_FLOAT, 1);
    ivblayout.Push(GL_FLOAT, 1);

    m_VAO->AddVBuffer(*m_VertexBuffer, vblayout);
    m_VAO->AddIBuffer(*m_InstanceBuffer, ivblayout);
    GLCall(glVertexAttribDivisor(2, 1);)
    GLCall(glVertexAttribDivisor(3, 1);)
    GLCall(glVertexAttribDivisor(4, 1);)

    m_IndexBuffer = std::make_unique<IndexBuffer>(indices, 6);
    m_Shader = std::make_unique<Shader>("../../res/shaders/Snake.shader");
    m_Shader->Bind();
    m_Texture = std::make_unique<Texture>();
    m_Texture->PathGenerate("../../res/textures/SnakeAtlas.png");
    m_Texture->Bind(0);
    m_Shader->SetUniform1i("u_textureatlas", 0);
    glDisable(GL_BLEND);
}

Snake::~Snake()
{
}

void Snake::print()
{
    std::cout << "---\n";
    std::cout << m_SnakeData.InputLayer[0].State << " Direkt Sagda Duvar Var Mi?\n";
    std::cout << m_SnakeData.InputLayer[1].State << " Direkt Solda Duvar Var Mi?\n";
    std::cout << m_SnakeData.InputLayer[2].State << " Direkt Ustte Duvar Var Mi?\n";
    std::cout << m_SnakeData.InputLayer[3].State << " Direkt Altta Duvar Var Mi?\n";
    std::cout << "---\n";
    std::cout << m_SnakeData.InputLayer[4].State << " Direkt Sagda Bedeni Var Mi?\n";
    std::cout << m_SnakeData.InputLayer[5].State << " Direkt Solda Bedeni Var Mi?\n";
    std::cout << m_SnakeData.InputLayer[6].State << " Direkt Ustte Bedeni Var Mi?\n";
    std::cout << m_SnakeData.InputLayer[7].State << " Direkt Altta Bedeni Var Mi?\n";
    std::cout << "---\n";
    std::cout << m_SnakeData.InputLayer[8].State <<  " Sagda Yem Var Mi?\n";
    std::cout << m_SnakeData.InputLayer[9].State <<  " Solda Yem Var Mi?\n";
    std::cout << m_SnakeData.InputLayer[10].State << " Ustte Yem Var Mi?\n";
    std::cout << m_SnakeData.InputLayer[11].State << " Altta Yem Var Mi?\n";
    std::cout << "---\n";
    std::cout << m_SnakeData.InputLayer[12].State << " Yon Sag Mi?\n";
    std::cout << m_SnakeData.InputLayer[13].State << " Yon Sol Mi?\n";
    std::cout << m_SnakeData.InputLayer[14].State << " Yon Ust Mi?\n";
    std::cout << m_SnakeData.InputLayer[15].State << " Yon Alt Mi?\n";
    std::cin.get();
}

void Snake::OnUpdate(float deltaTime)
{
    if (m_Start)
    {
        m_Accumulator += deltaTime;

        if (m_Accumulator >= m_MoveTime)
        {
            m_Accumulator = m_MoveTime;
            Smooth();
            m_Accumulator = 0;
            m_LifeTime -= 1;
            
            //print();
            
            if (GameStep()) return;
        }
        
        Smooth();
    } else
    {
        WakeUpGame();
        if (m_Machine)
        {
            WaitMachine();
            m_GameData->ready_machine = false;

            m_SnakeData = *m_BestOneData;
            std::cout << "\nGENERATION: (((" << m_SnakeData.ID * REPEAT << ")))\n";
            m_Start = true;
        }
    }

    if (!m_Machine)
        ChangeDirection();
}

void Snake::OnRender()
{   
    OtherInputs();

    m_InstanceBuffer->Data(m_Snake.data(), m_Snake.size() * sizeof(glm::vec4), GL_DYNAMIC_DRAW);

    m_Shader->Bind();
    m_Shader->SetUniform2f("u_translate", m_UIData->TranslateSnake.x, m_UIData->TranslateSnake.y);
    m_Shader->SetUniformMat4f("u_proj", m_Data->projection);

    DrawInstanced(*m_VAO, *m_IndexBuffer, *m_Shader, m_Snake.size());
}

void Snake::Move()
{
    switch (m_Direction)
    {
    case GLFW_KEY_RIGHT:
        m_Snake.push_back({m_Snake.back().x, m_Snake.back().y, HEAD, 0});
        if (!Cross())
        {
            m_Snake.at(m_Snake.size() - 2).z = HALF_BODY;
            m_Snake.at(m_Snake.size() - 2).w = 0;
            m_MakeBody = {BODY, 0};
        }
        m_WishHead = {m_Snake.back().x + m_GameData->grid_size, m_Snake.back().y};
        m_PrevTail = {m_Snake.at(1).x, m_Snake.at(1).y};
        m_Prev = GLFW_KEY_RIGHT;
        break;

    case GLFW_KEY_LEFT:
        m_Snake.push_back({m_Snake.back().x, m_Snake.back().y, 1, 2});
        if (!Cross())
        {
            m_Snake.at(m_Snake.size() - 2).z = HALF_BODY;
            m_Snake.at(m_Snake.size() - 2).w = 2;
            m_MakeBody = {BODY, 2};
        }
        m_WishHead = {m_Snake.back().x - m_GameData->grid_size, m_Snake.back().y};
        m_PrevTail = {m_Snake.at(1).x, m_Snake.at(1).y};
        m_Prev = GLFW_KEY_LEFT;
        break;
        
    case GLFW_KEY_UP:
        m_Snake.push_back({m_Snake.back().x, m_Snake.back().y, 1, 1});
        if (!Cross())
        {
            m_Snake.at(m_Snake.size() - 2).z = HALF_BODY;
            m_Snake.at(m_Snake.size() - 2).w = 1;
            m_MakeBody = {BODY, 1};
        }
        m_WishHead = {m_Snake.back().x, m_Snake.back().y + m_GameData->grid_size};
        m_PrevTail = {m_Snake.at(1).x, m_Snake.at(1).y};
        m_Prev = GLFW_KEY_UP;
        break;
        
    case GLFW_KEY_DOWN:
        m_Snake.push_back({m_Snake.back().x, m_Snake.back().y, 1, 3});
        if (!Cross())
        {
            m_Snake.at(m_Snake.size() - 2).z = HALF_BODY;
            m_Snake.at(m_Snake.size() - 2).w = 3;
            m_MakeBody = {BODY, 3};
        }
        m_WishHead = {m_Snake.back().x, m_Snake.back().y - m_GameData->grid_size};
        m_PrevTail = {m_Snake.at(1).x, m_Snake.at(1).y};
        m_Prev = GLFW_KEY_DOWN;
        break;
        
    default:
        return;
        break;
    }
}

void Snake::ChangeDirection()
{
    unsigned int key;

    if (m_Machine)
    {    
        EvaluateInputs();
        key = MakeDecision();
    } else {
        key = m_Data->key;
    }
    
    switch (key)
    {
    case GLFW_KEY_RIGHT:
        if (m_Prev != GLFW_KEY_LEFT)
        {
            m_Direction = GLFW_KEY_RIGHT;
            m_Start = true;
        }
        break;
    
    case GLFW_KEY_LEFT:
        if (m_Prev != GLFW_KEY_RIGHT)
        {
            m_Direction = GLFW_KEY_LEFT;
            m_Start = true;
        }
        break;
    
    case GLFW_KEY_UP:
        if (m_Prev != GLFW_KEY_DOWN)
        {
            m_Direction = GLFW_KEY_UP;
            m_Start = true;
        }
        break;
    
    case GLFW_KEY_DOWN:
        if (m_Prev != GLFW_KEY_UP)
        {
            m_Direction = GLFW_KEY_DOWN;
            m_Start = true;
        }
        break;

    default:
        break;
    }
}

void Snake::OtherInputs()
{
    switch (m_Data->key)
    {
    case GLFW_KEY_M:
        m_Machine = true;
        break;
    
    case GLFW_KEY_N:
        m_Machine = false;
        break;

    case GLFW_KEY_Z:
        m_MoveTime = 60;
        break;

    case GLFW_KEY_X:
        m_MoveTime = 10;
        break;

    case GLFW_KEY_C:
        if (m_GameData->machine_print)
        {
            m_GameData->machine_print = false;
        } else
        {
            m_GameData->machine_print = true;
        }
        break;

    /* case GLFW_KEY_A:
        m_GameData->dimension = 8;
        break;

    case GLFW_KEY_S:
        m_GameData->dimension = 16;
        break; */


    default:
        break;
    }
}

bool Snake::Collision()
{
    // Food
    if (m_Snake.at(0).x == m_Snake.back().x && m_Snake.at(0).y == m_Snake.back().y)
    {
        Food();
        return false;
    }

    // Border
    if (m_Snake.back().x == m_GameData->grid_width * m_GameData->grid_size || m_Snake.back().x == (m_GameData->grid_width - m_GameData->dimension - 1) * m_GameData->grid_size)
    {
        Lost();
        return true;
    }
    
    if (m_Snake.back().y == m_GameData->grid_height * m_GameData->grid_size || m_Snake.back().y == (m_GameData->grid_height - m_GameData->dimension - 1) * m_GameData->grid_size)
    {
        Lost();
        return true;
    }
    
    // Body
    for (int i = 1; i < m_Snake.size() - 4; i++)
    {
        if (m_Snake.at(i).x == m_Snake.back().x && m_Snake.at(i).y == m_Snake.back().y)
        {   
            Lost();
            return true;
        }
    }

    return false;
}

void Snake::Erase()
{
    if (!m_Food)
    {
        m_Snake.erase(m_Snake.begin() + 2);
    }
    m_Food = false;
}

void Snake::Food()
{
    bool ok = true;
    glm::vec4 food;
    m_LifeTime += LIFEADD;

    do
    {
        food.x = (rand() % m_GameData->grid_width) * m_GameData->grid_size;
        food.y = (rand() % m_GameData->grid_height) * m_GameData->grid_size;
        food.z = APPLE;
        food.w = 0;
        ok = false;

        for (int i = 0; i < m_Snake.size(); i++)
        {
            if (m_Snake.at(i).x == food.x && m_Snake.at(i).y == food.y)
            {
                ok = true;
                break;
            }
        }
        
    } while (ok);
    
    m_Food = true;
    m_Snake.at(0) = food;
}

void Snake::Smooth()
{   
    float smooth = m_Accumulator/(float)m_MoveTime;

    m_Snake.at(m_Snake.size() - 1).x = ((1 - smooth) * m_Snake.at(m_Snake.size() - 2).x) + (smooth * m_WishHead.x);
    m_Snake.at(m_Snake.size() - 1).y = ((1 - smooth) * m_Snake.at(m_Snake.size() - 2).y) + (smooth * m_WishHead.y);

    m_Snake.at(1).x = ((1 - smooth) * m_PrevTail.x) + (smooth * m_Snake.at(2).x);
    m_Snake.at(1).y = ((1 - smooth) * m_PrevTail.y) + (smooth * m_Snake.at(2).y);

    if (smooth > 0.3)
    {
        m_Snake.at(m_Snake.size() - 2).z = m_MakeBody.x;
        m_Snake.at(m_Snake.size() - 2).w = m_MakeBody.y;
    }

    if (smooth < 0.5)
    {
        m_Snake.at(1).w = Where(m_Snake.at(2).x, m_Snake.at(2).y, m_PrevTail.x, m_PrevTail.y, m_Snake.at(1).w);
    } else
    {
        if (m_Snake.at(2).z == CROSS || m_Snake.at(2).z == HALF_CROSS)
        {
            m_Snake.at(2).z = HALF_CROSS;
            m_Snake.at(2).w = Where(m_Snake.at(2).x, m_Snake.at(2).y, m_Snake.at(3).x, m_Snake.at(3).y, m_Snake.at(2).w);
        } else
        {
            m_Snake.at(2).z = HALF_BODY;
            m_Snake.at(2).w = Where(m_Snake.at(2).x, m_Snake.at(2).y, m_Snake.at(3).x, m_Snake.at(3).y, m_Snake.at(2).w);
        }
    }

    if (m_Snake.at(2).z == CROSS || m_Snake.at(2).z == HALF_CROSS)
    {
        if (smooth > 0.69)
        {
            m_Snake.at(1).w = Where(m_Snake.at(3).x, m_Snake.at(3).y, m_Snake.at(2).x, m_Snake.at(2).y, m_Snake.at(1).w);
            m_PrevTail.x = m_Snake.at(2).x;
            m_PrevTail.y = m_Snake.at(2).y;
            
            m_Snake.at(1).x = m_Snake.at(2).x;
            m_Snake.at(1).y = m_Snake.at(2).y;
            m_Snake.at(2).z = HALF_BODY;
        }
    }
}

bool Snake::Cross()
{
    if (m_Direction == m_Prev)
    {
        return false;
    } else
    {
        switch (m_Direction)
        {
        case GLFW_KEY_RIGHT:
            if (m_Prev == GLFW_KEY_UP)
            {
                // Up -> Right
                m_Snake.at(m_Snake.size() - 2).z = HALF_BODY;
                m_Snake.at(m_Snake.size() - 2).w = 1;
                m_MakeBody = {CROSS, 2};
            } else
            {
                // Down -> Right
                m_Snake.at(m_Snake.size() - 2).z = HALF_BODY;
                m_Snake.at(m_Snake.size() - 2).w = 3;
                m_MakeBody = {CROSS, 3};
            }
            
            break;
        
        case GLFW_KEY_LEFT:
            if (m_Prev == GLFW_KEY_UP)
            {
                // Up -> Left
                m_Snake.at(m_Snake.size() - 2).z = HALF_BODY;
                m_Snake.at(m_Snake.size() - 2).w = 1;
                m_MakeBody = {CROSS, 1};
            } else
            {
                // Down -> Left
                m_Snake.at(m_Snake.size() - 2).z = HALF_BODY;
                m_Snake.at(m_Snake.size() - 2).w = 3;
                m_MakeBody = {CROSS, 0};
            }

            break;
        
        case GLFW_KEY_UP:
            if (m_Prev == GLFW_KEY_RIGHT)
            {
                // Right -> Up
                m_Snake.at(m_Snake.size() - 2).z = HALF_BODY;
                m_Snake.at(m_Snake.size() - 2).w = 0;
                m_MakeBody = {CROSS, 0};
            } else
            {
                // Left -> Up
                m_Snake.at(m_Snake.size() - 2).z = HALF_BODY;
                m_Snake.at(m_Snake.size() - 2).w = 2;
                m_MakeBody = {CROSS, 3};
            }

            break;
        
        case GLFW_KEY_DOWN:
            if (m_Prev == GLFW_KEY_RIGHT)
            {
                // Right -> Down
                m_Snake.at(m_Snake.size() - 2).z = HALF_BODY;
                m_Snake.at(m_Snake.size() - 2).w = 0;
                m_MakeBody = {CROSS, 1};
            } else
            {
                // Left -> Down
                m_Snake.at(m_Snake.size() - 2).z = HALF_BODY;
                m_Snake.at(m_Snake.size() - 2).w = 2;
                m_MakeBody = {CROSS, 2};
            }

            break;
        
        default:
            return false;
            break;
        }
        return true;
    }
    return false;
}

int Snake::Where(int Thisx, int Thisy, int toThisx, int toThisy, int fallback)
{
    if (Thisx == toThisx)
    {
        if (Thisy < toThisy)
        {   // DOWN
            return 3;
        } else if (Thisy > toThisy)
        {   // UP
            return 1;
        }
    } else if (Thisx < toThisx)
    {   // LEFT
        return 2;
    } else if (Thisx > toThisx)
    {   //  RIGHT
        return 0;
    }
    return fallback;
}

void Snake::ResetSnake()
{
    m_Start = false;
    m_Accumulator = 0;
    m_Data->key = 0;
    m_Direction = GLFW_KEY_UP;
    m_Prev = GLFW_KEY_UP;
    m_MakeBody = {BODY, 1};
    
    int initX = m_GameData->grid_width / 2 - 1;
    int initY = m_GameData->grid_height / 2 - 1;
    m_Snake.clear();
    m_Snake.push_back({0, 0, APPLE, 0});                                                                 // Apple
    m_Snake.push_back({initX * m_GameData->grid_size, (initY - 2) * m_GameData->grid_size, TAIL, 1});    // Tail
    m_Snake.push_back({initX * m_GameData->grid_size, (initY - 1) * m_GameData->grid_size, EMPTY, 1});   // Last Body
    m_Snake.push_back({initX * m_GameData->grid_size, (initY - 1) * m_GameData->grid_size, BODY, 1});    // Body
    m_Snake.push_back({initX * m_GameData->grid_size, (initY    ) * m_GameData->grid_size, BODY, 1});    // Body
    m_Snake.push_back({initX * m_GameData->grid_size, (initY + 1) * m_GameData->grid_size, HEAD, 1});    // Head
    Food();
    m_Food = false;
    m_WishHead = {m_Snake.back().x, m_Snake.back().y};
    m_PrevTail = {m_Snake.at(1).x, m_Snake.at(1).y};
}

bool Snake::GameStep()
{
    if (m_LifeTime <= 0)
    {
        Lost();
        return true;
    }
    
    if (Collision()) return true;
    ChangeDirection();
    Move();
    Erase();

    return false;
}

void Snake::Lost()
{
    m_LifeTime = LIFETIME;
    m_Machine = true;
    std::cout << "\nSNAKE SCORE: " << m_Snake.size() - 6 << '\n';
    ResetSnake();
}

unsigned int Snake::MakeDecision()
{
    // Flush The Values
    for (int i = 0; i < FIRST_LAYER; i++)
    {
        m_SnakeData.FirstLayer[i].Value = 0;
    }

    for (int i = 0; i < SECOND_LAYER; i++)
    {
        m_SnakeData.SecondLayer[i].Value = 0;
    }

    for (int i = 0; i < OUTPUT_LAYER; i++)
    {
        m_SnakeData.OutputLayer[i].Value = 0;
    }
    
    // Make The Decision
    for (int i = 0; i < FIRST_LAYER; i++)
    {
        for (int j = 0; j < INPUT_LAYER; j++)
        {
            m_SnakeData.FirstLayer[i].Value += m_SnakeData.FirstLayer[i].Weights[j] * m_SnakeData.InputLayer[j].State;
        }
        m_SnakeData.FirstLayer[i].Value += m_SnakeData.FirstLayer[i].Bias;

        // RELU
        if (m_SnakeData.FirstLayer[i].Value < 0)
        {
            m_SnakeData.FirstLayer[i].Value = 0;
        }
    }

    for (int i = 0; i < SECOND_LAYER; i++)
    {
        for (int j = 0; j < FIRST_LAYER; j++)
        {
            m_SnakeData.SecondLayer[i].Value += m_SnakeData.SecondLayer[i].Weights[j] * m_SnakeData.FirstLayer[j].Value;
        }
        m_SnakeData.SecondLayer[i].Value += m_SnakeData.SecondLayer[i].Bias;

        // RELU
        if (m_SnakeData.SecondLayer[i].Value < 0)
        {
            m_SnakeData.SecondLayer[i].Value = 0;
        }
    }

    for (int i = 0; i < OUTPUT_LAYER; i++)
    {
        for (int j = 0; j < SECOND_LAYER; j++)
        {
            m_SnakeData.OutputLayer[i].Value += m_SnakeData.OutputLayer[i].Weights[j] * m_SnakeData.SecondLayer[j].Value;
        }
        m_SnakeData.OutputLayer[i].Value += m_SnakeData.OutputLayer[i].Bias;

        // RELU
        if (m_SnakeData.OutputLayer[i].Value < 0)
        {
            m_SnakeData.OutputLayer[i].Value = 0;
        }
    }

    float sum = 0;

    for (int i = 0; i < OUTPUT_LAYER; i++)
    {
        sum += m_SnakeData.OutputLayer[i].Value;
    }

    float probability = 0;
    float random = static_cast<float>(rand()) / RAND_MAX;

    for (int i = 0; i < OUTPUT_LAYER; i++)
    {
        if (probability <= random && random < probability + (m_SnakeData.OutputLayer[i].Value / sum))
        {
            switch (i)
            {
                case 0:
                return GLFW_KEY_RIGHT;
                break;
                case 1:
                return GLFW_KEY_LEFT;
                break;
                case 2:
                return GLFW_KEY_UP;
                break;
                case 3:
                return GLFW_KEY_DOWN;
                break;
                default:
                break;
            }
            break;
        }
        probability += m_SnakeData.OutputLayer[i].Value / sum;
    }
    return m_Prev;
}

void Snake::EvaluateInputs()
{
    // Sağ Beden
    int look = m_Snake.back().x;
    bool stop = false;
    while (look < m_GameData->grid_width * m_GameData->grid_size)
    {
        look += m_GameData->grid_size;
        for (int i = 1; i < m_Snake.size() - 1; i++)
        {
            if (look == m_Snake.at(i).x && m_Snake.back().y == m_Snake.at(i).y)
            {
                m_SnakeData.InputLayer[0].State = 1 - ((float)(look - (m_Snake.back().x + m_GameData->grid_size)) / (m_GameData->grid_width * m_GameData->grid_size));
                stop = true;
                break;
            }
            m_SnakeData.InputLayer[0].State = 0;
        }
        if (stop) break;
    }

    // Sol Beden
    look = m_Snake.back().x;
    stop = false;
    while (look >= 0)
    {
        look -= m_GameData->grid_size;
        for (int i = 1; i < m_Snake.size() - 1; i++)
        {
            if (look == m_Snake.at(i).x && m_Snake.back().y == m_Snake.at(i).y)
            {
                m_SnakeData.InputLayer[1].State = 1 - ((float)((m_Snake.back().x - m_GameData->grid_size) - look) / (m_GameData->grid_width * m_GameData->grid_size));
                stop = true;
                break;
            }
            m_SnakeData.InputLayer[1].State = 0;
        }
        if (stop) break;
    }

    // Üst Beden
    look = m_Snake.back().y;
    stop = false;
    while (look < m_GameData->grid_height * m_GameData->grid_size)
    {
        look += m_GameData->grid_size;
        for (int i = 1; i < m_Snake.size() - 1; i++)
        {
            if (look == m_Snake.at(i).y && m_Snake.back().x == m_Snake.at(i).x)
            {
                m_SnakeData.InputLayer[2].State = 1 - ((float)(look - (m_Snake.back().y + m_GameData->grid_size)) / (m_GameData->grid_width * m_GameData->grid_size));
                stop = true;
                break;
            }
            m_SnakeData.InputLayer[2].State = 0;
        }
        if (stop) break;
    }

    // Alt Beden
    look = m_Snake.back().y;
    stop = false;
    while (look >= 0)
    {
        look -= m_GameData->grid_size;
        for (int i = 1; i < m_Snake.size() - 1; i++)
        {
            if (look == m_Snake.at(i).y && m_Snake.back().x == m_Snake.at(i).x)
            {
                m_SnakeData.InputLayer[3].State = 1 - ((float)((m_Snake.back().y - m_GameData->grid_size) - look) / (m_GameData->grid_width * m_GameData->grid_size));
                stop = true;
                break;
            }
            m_SnakeData.InputLayer[3].State = 0;
        }
        if (stop) break;
    }

    // Sağda yem var mı?
    if (m_Snake.back().x < m_Snake.at(0).x && m_Snake.back().y == m_Snake.at(0).y)
    {
        m_SnakeData.InputLayer[4].State = 1 - ((float)(m_Snake.at(0).x - (m_Snake.back().x + m_GameData->grid_size)) / (m_GameData->grid_width * m_GameData->grid_size));
    } else { m_SnakeData.InputLayer[4].State = 0; }

    // Solda yem var mı?
    if (m_Snake.back().x > m_Snake.at(0).x && m_Snake.back().y == m_Snake.at(0).y)
    {
        m_SnakeData.InputLayer[5].State = 1 - ((float)((m_Snake.back().x - m_GameData->grid_size) - m_Snake.at(0).x) / (m_GameData->grid_width * m_GameData->grid_size));
    } else { m_SnakeData.InputLayer[5].State = 0; }

    // Üstte yem var mı?
    if (m_Snake.back().x == m_Snake.at(0).x && m_Snake.back().y < m_Snake.at(0).y)
    {
        m_SnakeData.InputLayer[6].State = 1 - ((float)(m_Snake.at(0).y - (m_Snake.back().y + m_GameData->grid_size)) / (m_GameData->grid_height * m_GameData->grid_size));
    } else { m_SnakeData.InputLayer[6].State = 0; }

    // Altta yem var mı?
    if (m_Snake.back().x == m_Snake.at(0).x && m_Snake.back().y > m_Snake.at(0).y)
    {
        m_SnakeData.InputLayer[7].State = 1 - ((float)((m_Snake.back().y - m_GameData->grid_size) - m_Snake.at(0).y) / (m_GameData->grid_height * m_GameData->grid_size));
    } else { m_SnakeData.InputLayer[7].State = 0; }

    int x_difference = m_Snake.at(0).x - m_Snake.back().x;
    int y_difference = m_Snake.at(0).y - m_Snake.back().y;

    // Sağ üstte yem var mı?
    if ((x_difference == y_difference) && (x_difference >= 0))
    {
        m_SnakeData.InputLayer[8].State = 1 - ((float)(x_difference + y_difference - 2 * m_GameData->grid_size) / ((m_GameData->grid_width + m_GameData->grid_height) * m_GameData->grid_size));
    } else { m_SnakeData.InputLayer[8].State = 0; }

    // Sol üstte yem var mı?
    if ((- x_difference == y_difference) && (y_difference >= 0))
    {
        m_SnakeData.InputLayer[9].State = 1 - ((float)(- x_difference + y_difference - 2 * m_GameData->grid_size) / ((m_GameData->grid_width + m_GameData->grid_height) * m_GameData->grid_size));
    } else { m_SnakeData.InputLayer[9].State = 0; }

    // Sağ altta yem var mı?
    if ((x_difference == - y_difference) && (y_difference <= 0))
    {
        m_SnakeData.InputLayer[10].State = 1 - ((float)(x_difference - y_difference - 2 * m_GameData->grid_size) / ((m_GameData->grid_width + m_GameData->grid_height) * m_GameData->grid_size));
    } else { m_SnakeData.InputLayer[10].State = 0; }

    // Sol altta yem var mı?
    if ((x_difference == y_difference) && (x_difference <= 0))
    {
        m_SnakeData.InputLayer[11].State = 1 - ((float)(- x_difference - y_difference - 2 * m_GameData->grid_size) / ((m_GameData->grid_width + m_GameData->grid_height) * m_GameData->grid_size));
    } else { m_SnakeData.InputLayer[11].State = 0; }

    // Sağ Duvar
    m_SnakeData.InputLayer[12].State = 1 - ((float)((m_GameData->grid_width * m_GameData->grid_size) - (m_Snake.back().x + m_GameData->grid_size)) / (m_GameData->grid_width * m_GameData->grid_size));

    // Sol Duvar
    m_SnakeData.InputLayer[13].State = 1 - ((float)(m_Snake.back().x) / (m_GameData->grid_width * m_GameData->grid_size));

    // Üst Duvar
    m_SnakeData.InputLayer[14].State = 1 - ((float)((m_GameData->grid_width * m_GameData->grid_size) - (m_Snake.back().y + m_GameData->grid_size)) / (m_GameData->grid_height * m_GameData->grid_size));

    // Alt Duvar
    m_SnakeData.InputLayer[15].State = 1 - ((float)(m_Snake.back().y) / (m_GameData->grid_height * m_GameData->grid_size));
    
    // Yılanın yönü sağ mı?
    if (m_Direction == GLFW_KEY_RIGHT)
    {
        m_SnakeData.InputLayer[16].State = 1;
    } else { m_SnakeData.InputLayer[16].State = 0; }

    // Yılanın yönü sol mu?
    if (m_Direction == GLFW_KEY_LEFT)
    {
        m_SnakeData.InputLayer[17].State = 1;
    } else { m_SnakeData.InputLayer[17].State = 0; }

    // Yılanın yönü yukarı mı?
    if (m_Direction == GLFW_KEY_UP)
    {
        m_SnakeData.InputLayer[18].State = 1;
    } else { m_SnakeData.InputLayer[18].State = 0; }

    // Yılanın yönü aşağı mı?
    if (m_Direction == GLFW_KEY_DOWN)
    {
        m_SnakeData.InputLayer[19].State = 1;
    } else { m_SnakeData.InputLayer[19].State = 0; }

    /* // Direkt sağında duvar var mı?
    if (m_Snake.back().x + m_GameData->grid_size == m_GameData->grid_width * m_GameData->grid_size)
    {
        m_SnakeData.InputLayer[0].State = 1;
    } else { m_SnakeData.InputLayer[0].State = 0; }

    // Direkt solunda duvar var mı?
    if (m_Snake.back().x - m_GameData->grid_size == (m_GameData->grid_width - m_GameData->dimension - 1) * m_GameData->grid_size)
    {
        m_SnakeData.InputLayer[1].State = 1;
    } else { m_SnakeData.InputLayer[1].State = 0; }

    // Direkt üstünde duvar var mı?
    if (m_Snake.back().y + m_GameData->grid_size == m_GameData->grid_height * m_GameData->grid_size)
    {
        m_SnakeData.InputLayer[2].State = 1;
    } else { m_SnakeData.InputLayer[2].State = 0; }

    // Direkt altında duvar var mı?
    if (m_Snake.back().y - m_GameData->grid_size == (m_GameData->grid_height - m_GameData->dimension - 1) * m_GameData->grid_size)
    {
        m_SnakeData.InputLayer[3].State = 1;
    } else { m_SnakeData.InputLayer[3].State = 0; }

    // Beden
    for (int i = 1; i < m_Snake.size() - 1; i++)
    {
        // Direkt sağında bedeni var mı?
        if (m_Snake.back().x + m_GameData->grid_size == m_Snake.at(i).x && m_Snake.back().y == m_Snake.at(i).y)
        {
            m_SnakeData.InputLayer[4].State = 1;
        } else { m_SnakeData.InputLayer[4].State = 0; }

        // Direkt solunda bedeni var mı?
        if (m_Snake.back().x - m_GameData->grid_size == m_Snake.at(i).x && m_Snake.back().y == m_Snake.at(i).y)
        {
            m_SnakeData.InputLayer[5].State = 1;
        } else { m_SnakeData.InputLayer[5].State = 0; }

        // Direkt üstünde bedeni var mı?
        if (m_Snake.back().x == m_Snake.at(i).x && m_Snake.back().y + m_GameData->grid_size == m_Snake.at(i).y)
        {
            m_SnakeData.InputLayer[6].State = 1;
        } else { m_SnakeData.InputLayer[6].State = 0; }

        // Direkt altında bedeni var mı?
        if (m_Snake.back().x == m_Snake.at(i).x && m_Snake.back().y - m_GameData->grid_size == m_Snake.at(i).y)
        {
            m_SnakeData.InputLayer[7].State = 1;
        } else { m_SnakeData.InputLayer[7].State = 0; }
    }

    // Sağda yem var mı?
    if (m_Snake.back().x < m_Snake.at(0).x && m_Snake.back().y == m_Snake.at(0).y)
    {
        m_SnakeData.InputLayer[8].State = 1;
    } else { m_SnakeData.InputLayer[8].State = 0; }

    // Solda yem var mı?
    if (m_Snake.back().x > m_Snake.at(0).x && m_Snake.back().y == m_Snake.at(0).y)
    {
        m_SnakeData.InputLayer[9].State = 1;
    } else { m_SnakeData.InputLayer[9].State = 0; }

    // Üstte yem var mı?
    if (m_Snake.back().x == m_Snake.at(0).x && m_Snake.back().y < m_Snake.at(0).y)
    {
        m_SnakeData.InputLayer[10].State = 1;
    } else { m_SnakeData.InputLayer[10].State = 0; }

    // Altta yem var mı?
    if (m_Snake.back().x == m_Snake.at(0).x && m_Snake.back().y > m_Snake.at(0).y)
    {
        m_SnakeData.InputLayer[11].State = 1;
    } else { m_SnakeData.InputLayer[11].State = 0; }
    
    // Yılanın yönü sağ mı?
    if (m_Direction == GLFW_KEY_RIGHT)
    {
        m_SnakeData.InputLayer[12].State = 1;
    } else { m_SnakeData.InputLayer[12].State = 0; }

    // Yılanın yönü sol mu?
    if (m_Direction == GLFW_KEY_LEFT)
    {
        m_SnakeData.InputLayer[13].State = 1;
    } else { m_SnakeData.InputLayer[13].State = 0; }

    // Yılanın yönü yukarı mı?
    if (m_Direction == GLFW_KEY_UP)
    {
        m_SnakeData.InputLayer[14].State = 1;
    } else { m_SnakeData.InputLayer[14].State = 0; }

    // Yılanın yönü aşağı mı?
    if (m_Direction == GLFW_KEY_DOWN)
    {
        m_SnakeData.InputLayer[15].State = 1;
    } else { m_SnakeData.InputLayer[15].State = 0; } */
}

void Snake::WaitMachine()
{
    std::unique_lock<std::mutex> lock(mtx);
    cv.wait(lock, [this] { return m_GameData->ready_machine; });
}

void Snake::WakeUpGame()
{
    {
        std::lock_guard<std::mutex> lock(mtx);
        m_GameData->ready_game = true;
    }
    cv.notify_one();
}

void Snake::Debug(std::string message)
{
    std::cout << m_Snake.size() << message <<'\n';
    for (int i = 0; i < m_Snake.size(); i++)
    {
        std::cout << m_Snake.at(i).x << " " << m_Snake.at(i).y << " " << m_Snake.at(i).z << " " << m_Snake.at(i).w << "     ";

        switch ((int)m_Snake.at(i).z)
        {
        case 0:
            std::cout << "Apple" << '\n';
            break;
        
        case 1:
            std::cout << "Head" << '\n';
            break;
        
        case 2:
            std::cout << "Body" << '\n';
            break;
        
        case 3:
            std::cout << "Half Body" << '\n';
            break;
        
        case 4:
            std::cout << "Cross" << '\n';
            break;
        
        case 5:
            std::cout << "Tail" << '\n';
            break;
        
        default:
            break;
        }
    }
}

/*
Textures:

    Apple      : 0    0.0, 0.75
    Head       : 1   0.25, 0.75
    Tail       : 2    0.5, 0.75
    Half-Body  : 3   0.75, 0.75
    Cross      : 4    0.0,  0.5
    Body       : 5   0.25,  0.5
    Half-Cross : 6    0.5,  0.5

    Rotate Counter-Clockwise
    Don't : 0   RIGHT
    90    : 1   UP
    180   : 2   LEFT
    270   : 3   DOWN

    x     y     z    w
    posx, posy, tex, rot

*/