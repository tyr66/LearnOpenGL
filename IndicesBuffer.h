#pragma once
#include <iostream>
#include <memory>

class IndiceBuffer {
private:
    IndiceBuffer(unsigned int id, unsigned int* buffer, int count);
public:
    static std::unique_ptr<IndiceBuffer> CreateIndiceBuffer(unsigned int* buffer , int count);
    ~IndiceBuffer();
    void Bind() const;
    void Unbind() const;
private:
    unsigned int _renderID{0};
    unsigned int* _data;
    unsigned int _count;
};
