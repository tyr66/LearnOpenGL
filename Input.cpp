#include <cassert>
#include <glfw/glfw3.h>

#include "Input.h"

float xMouse;
float yMouse;
float xOffset;
float yOffset;
GLFWwindow* _window;

static void mouse_callback(GLFWwindow* window, double xmouse, double ymouse)
{
    xMouse = xmouse;
    yMouse = ymouse;
}

static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    xOffset = xoffset;
    yOffset = yoffset;
}

void Input::Init(GLFWwindow* window)
{
    assert(window != nullptr);
    _window = window;

    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    double _x, _y;
    glfwGetCursorPos(window, &_x, &_y);
    xMouse = (float)_x;
    yMouse = (float)_y;
}

float Input::getXMouse()
{
    return xMouse;
}

float Input::getYMouse()
{
    return yMouse;
}

float Input::getXoffset()
{
    return xOffset;
}

float Input::getYoffste()
{
    return yOffset;
}

bool Input::IsKeyPress(int key)
{
    return glfwGetKey(_window, key) == GLFW_PRESS;
}

void Input::Reset()
{
    yOffset = xOffset = 0;
}

