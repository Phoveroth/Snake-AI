#pragma once

#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "InstanceBuffer.h"
#include "Texture.h"
#include "ErrorManager.h"

#include <memory>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include <GLFW/glfw3.h>

#define APPLE 0
#define HEAD 1
#define TAIL 2
#define HALF_BODY 3
#define CROSS 4
#define BODY 5
#define HALF_CROSS 6
#define EMPTY 13

const int WIDTH = 800;
const int HEIGHT = 600;

#define INPUT_LAYER  20
#define FIRST_LAYER  20 //12
#define SECOND_LAYER 20 //8
#define OUTPUT_LAYER  4

struct NetworkValues {
    float input[INPUT_LAYER];
    float first[FIRST_LAYER];
    float second[SECOND_LAYER];
    float output[OUTPUT_LAYER];
};

struct WindowData {
    int width;
    int height;
    glm::mat4 projection;
    int key;
    bool pressed;
    double mouseX, mouseY;
};

struct UIData {
    glm::vec2 TranslateSnake;
    glm::vec2 TranslateNetwork;
};

struct GameData {
    float grid_size;
    int grid_width;
    int grid_height;
    int dimension;
    bool machine_print = false;
    bool set_values = false;
    NetworkValues network_values;
    bool running = true;
    bool debugMode = false;
};

struct Input
{
    float State;
};

struct First
{
    float Value = 0;
    float Weights[INPUT_LAYER];
    float Bias;
};

struct Second
{
    float Value = 0;
    float Weights[FIRST_LAYER];
    float Bias;
};

struct Output
{
    float Value = 0;
    float Weights[SECOND_LAYER];
    float Bias;
};

struct Life
{
    Input InputLayer[INPUT_LAYER];
    First FirstLayer[FIRST_LAYER];
    Second SecondLayer[SECOND_LAYER];
    Output OutputLayer[OUTPUT_LAYER];
    unsigned int ID;
};

inline long int Fitness_Function(unsigned int Food, int steps, bool isCollision)
{
    return ((Food * Food) + (20 * Food) + steps - ((int)isCollision * Food * 10)); // ((Food * Food) + (10 * Food) + steps - ((int)isCollision * Food * 10)); // (Food * Food * Food) + (500 * Food) - i - ((int)isLost * 250 * Food);
}