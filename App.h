#pragma  once

#include <vector>

struct GLFWwindow;

class App final {
public:
    App(int width, int height);
    void init();
    void run();
    ~App();
private:
    void initGlfw();
    void initBuffer();
    void initShader();
    void processInput();

private:
    int _width;
    int _height;
    GLFWwindow* _window{nullptr};
    unsigned int _shaderProgram{0};
    unsigned int _VBO;
    unsigned int _VAO;
    unsigned int _EBO;
    bool _initBuffer{false};
    bool _initShader{false};
};
