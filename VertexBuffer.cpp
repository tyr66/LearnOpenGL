#include "VertexBuffer.h"
#include "glad/glad.h"
#include "help.h"

VertexBuffer::VertexBuffer(unsigned int id, unsigned int count):_renderID(id), _count(count)
{
}

std::unique_ptr<VertexBuffer> VertexBuffer::CreateVertexBuffer(void* buffer , unsigned int size){

    unsigned int vbo = 0;
    GLCall(glGenBuffers(1, &vbo));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, vbo));
    GLCall(glBufferData(GL_ARRAY_BUFFER, size, buffer, GL_STATIC_DRAW));
    return std::unique_ptr<VertexBuffer>(new VertexBuffer(vbo, size));
}

VertexBuffer::~VertexBuffer()
{
    GLCall(glDeleteBuffers(1, &_renderID));
}

void VertexBuffer::Bind() const
{
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, _renderID));
}

void VertexBuffer::Unbind() const
{
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}
