#pragma  once

#include <vector>
#include "Camera.h"

struct GLFWwindow;

class App final {
public:
    App(int width, int height);
    void init();
    void run();
    void test();
    ~App();
private:
    void initGlfw();
    void initBuffer();
    void initShader();
    void processInput();


private:
    int _width;
    int _height;
    Camera _camera;
    GLFWwindow* _window{nullptr};
    float _xMouse;
    float _yMouse;
    float _xOffset;
    float _yOffset;
};
