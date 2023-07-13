
struct GLFWwindow;

class App final {
public:
    App(int width, int height);
    void init();
    void run();
    ~App();
private:
    void initGlfw();

    void processInput();

private:
    int _width;
    int _height;
    GLFWwindow* _window{nullptr};
};
