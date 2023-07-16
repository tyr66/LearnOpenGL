#pragma once
#include <iostream>
#include <memory>

class VertexBuffer {
private:
    VertexBuffer(unsigned int id, void* buffer, unsigned int count);
public:
    static std::unique_ptr<VertexBuffer> CreateVertexBuffer(void* buffer , unsigned int size);
    ~VertexBuffer();
    void Bind() const;
    void Unbind() const;
private:
    unsigned int _renderID;
    void* _data;
    unsigned int _count;
};
