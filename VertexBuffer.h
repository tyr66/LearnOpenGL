#pragma once
#include <iostream>
#include <memory>

class VertexBuffer {
private:
    VertexBuffer(unsigned int id, unsigned int count);
public:
    static std::unique_ptr<VertexBuffer> CreateVertexBuffer(void* buffer , unsigned int size);
    ~VertexBuffer();
    void Bind() const;
    void Unbind() const;
private:
    unsigned int _renderID;
    unsigned int _count;
};
