#pragma once

struct GLFWwindow;

class Input {
public:
    static void Init(GLFWwindow* window);
    static float getXMouse();
    static float getYMouse();
    static float getXoffset();
    static float getYoffste();
    static bool IsKeyPress(int key);
    static void Reset();
private:
};
