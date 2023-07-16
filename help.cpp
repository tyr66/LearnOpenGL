#include <iostream>
#include <stdexcept>

#include "help.h"
#include "glad/glad.h"

void errorHandle(const char* msg)
{
    throw std::runtime_error(msg);
}

void glClearError()
{
    while (glGetError() != GL_NO_ERROR);
}

bool glCheckError(const char* function, const char* file, int line)
{
    GLenum state;
    bool flag = true;
    while ((state = glGetError()) != GL_NO_ERROR)
    {
        std::cout << "[OpenGL Error] : error hex is " << std::hex <<  state << std::endl
                    << "\t" <<  "FILE: " << file << std::endl
                    << "\t" << "FUNCTION: " << function << std::endl
                    << "\t" << "LINE: " << std::dec << line << std::endl;
                    
        flag = false;
    }

    return flag;
}
