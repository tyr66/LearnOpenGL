#pragma once

#define ASSERT(x) if (!(x)) errorHandle("GL error");
#define GLCall(x) (glClearError(), (x)); ASSERT(glCheckError(#x, __FILE__, __LINE__))


void glClearError();

bool glCheckError(const char* function, const char* file, int line);

void errorHandle(const char* msg);
