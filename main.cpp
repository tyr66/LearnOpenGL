#include <iostream>
#include "App.h"

const int WIDTH = 800;
const int HEIGHT = 600; 

int main()
{
    App app(WIDTH, HEIGHT);
    app.init();
    app.run();
    // app.test();
    app.clear();

    return 0;
}
