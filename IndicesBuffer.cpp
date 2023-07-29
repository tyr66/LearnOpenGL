#include "IndicesBuffer.h"
#include "glad/glad.h"
#include "help.h"

IndiceBuffer::IndiceBuffer(unsigned int id, int count):_renderID(id), _count(count)
{
}

std::unique_ptr<IndiceBuffer> IndiceBuffer::CreateIndiceBuffer(unsigned int* buffer , int count){

    unsigned int indiceBufferID;
    GLCall(glGenBuffers(1, &indiceBufferID));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indiceBufferID));
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * count, buffer, GL_STATIC_DRAW));

    return std::unique_ptr<IndiceBuffer>(new IndiceBuffer(indiceBufferID, count));
}

IndiceBuffer::~IndiceBuffer()
{
    GLCall(glDeleteBuffers(1, &_renderID));
}

void IndiceBuffer::Bind() const
{
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _renderID));
}

void IndiceBuffer::Unbind() const
{
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}
