#include "Application.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    WindowData* data = static_cast<WindowData*>(glfwGetWindowUserPointer(window));

    glfwGetFramebufferSize(window, &(data->width), &(data->height));

    glViewport(0, 0, data->width, data->height);

    data->projection = glm::ortho(0.0f, (float)data->width, 0.0f, (float)data->height, -1.0f, 1.0f);
    
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    WindowData* data = static_cast<WindowData*>(glfwGetWindowUserPointer(window));

    if (action == GLFW_PRESS)
        data->key = key;

    if (key == GLFW_KEY_ESCAPE)
        glfwSetWindowShouldClose(window, true);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    WindowData* data = static_cast<WindowData*>(glfwGetWindowUserPointer(window));

    glfwGetCursorPos(window, &(data->mouseX), &(data->mouseY));
    data->mouseY = data->height - data->mouseY;

    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            data->pressed = true;
        } else if (action == GLFW_RELEASE) {
            data->pressed = false;
        }
    }
}

void cursor_position_callback(GLFWwindow* window, double mouseX, double mouseY) {
    WindowData* data = static_cast<WindowData*>(glfwGetWindowUserPointer(window));
    glfwGetCursorPos(window, &(data->mouseX), &(data->mouseY));
    data->mouseY = data->height - data->mouseY;
}

void Thread(GameData* gamedata, Life* bestonedata)
{
    srand(time(NULL));
    Machine brain(gamedata, bestonedata);
    brain.running = true;
    while (brain.running)
    {
        brain.Run();
    }
}

int main()
{
    if (!glfwInit()) return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Snake AI", NULL, NULL);
    if (window == NULL) return -1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) return -1;
    std::cout << glGetString(GL_VERSION) << std::endl;
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    GLCall(glEnable(GL_BLEND));
    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    glfwSetKeyCallback(window, key_callback);
    WindowData data{WIDTH, HEIGHT};
    data.projection = glm::ortho(0.0f, (float)WIDTH, 0.0f, (float)HEIGHT, -1.0f, 1.0f);
    glfwSetWindowUserPointer(window, &data);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    srand(time(NULL));
    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    std::cout << mode->width << " " << mode->height << " " << mode->refreshRate << "Hz" << '\n';

    GameData gamedata;
    gamedata.dimension = 16; // 8, 16
    gamedata.grid_size = mode->width / 64.0;
    gamedata.grid_width = gamedata.dimension;
    gamedata.grid_height = gamedata.dimension;
    gamedata.atlas_size = 0.25f;

    UIData uidata;
    uidata.TranslateSnake = {100, 100};
    uidata.TranslateNetwork = {500, 100};

    Life BestOneData;

    UserInterface interface(window, &data, &uidata, &gamedata);
    Snake snake(window, &data, &uidata, &gamedata, &BestOneData);
    Network nn(window, &data, &uidata, &gamedata);
    
    std::thread worker(Thread, &gamedata, &BestOneData);

    // Delta Time
    auto currentTime = std::chrono::steady_clock::now();
    auto lastTime = currentTime;
    float deltaTime;

    while(!glfwWindowShouldClose(window))
    {
        currentTime = std::chrono::steady_clock::now();
        deltaTime = std::chrono::duration<float, std::milli>(currentTime - lastTime).count();
        lastTime = currentTime;

        GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
        GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

        interface.OnUpdate(deltaTime);
        interface.OnRender();

        snake.OnUpdate(deltaTime);
        snake.OnRender();

        nn.OnUpdate(deltaTime);
        nn.OnRender();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    worker.join();

    return 0;
}




/* 
game logic
machine.randomparameters();
do
machine.calculateeachfitness();
bestone.getparameters(machine.selectbest());
snake.play(bestone(snake.inputs));
thread
machine.generatenewgeneration();
loop
 */