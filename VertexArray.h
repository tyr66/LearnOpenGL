#pragma once

#include <iostream>
#include <memory>
#include <vector>
#include "VertexBuffer.h"
#include "glad/glad.h"
#include <cassert>

class VertexBuffer;

struct ElementLayout {
    unsigned int type;
    unsigned int count;
    bool normalized;
    unsigned int idx;
    unsigned int divisor;

    unsigned int getTypeSize() const;
};

class VertexBufferLayout {
public:
    const std::vector<ElementLayout>& getElements()const {return _elements;}
    unsigned int getStride() const {return _stride;}
    template<typename T>
    void push(unsigned int count, unsigned int bindpoint, unsigned int divisor = 0);


private:
    std::vector<ElementLayout> _elements;
    unsigned int _stride{0};
};

class VertexArray{
private:
    VertexArray(unsigned int id);
public:

    static std::unique_ptr<VertexArray> CreateVertexArray();
    ~VertexArray();
    void AddBuffer(const VertexBuffer& vertexArr, const VertexBufferLayout& Layout);

    void Bind();
    void Unbind();

private:
    unsigned int _rendID;
};
