#include <iostream>
#include "App.h"
#include "Shader.h"

const int WIDTH = 800;
const int HEIGHT = 600; 

int main()
{
    App app(WIDTH, HEIGHT);
    app.init();
    app.run();

    ShaderGenerator::Clear();

    return 0;
}
