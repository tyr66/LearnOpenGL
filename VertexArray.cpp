#include "help.h"
#include "VertexArray.h"
#include "glad/glad.h"
#include "VertexBuffer.h"

unsigned int ElementLayout::getTypeSize() const
{
    switch(type)
    {
        case GL_FLOAT: return sizeof(GLfloat);
        case GL_UNSIGNED_INT: return sizeof(GLuint);
        case GL_UNSIGNED_BYTE: return sizeof(GLubyte);
    }

    assert(false);
    return 0;
}

VertexArray::VertexArray(unsigned int id): _rendID(id)
{
}

VertexArray::~VertexArray()
{
    GLCall(glDeleteVertexArrays(1, &_rendID));
}

std::unique_ptr<VertexArray> VertexArray::CreateVertexArray()
{
    unsigned int vao;
    GLCall(glGenVertexArrays(1, &vao));
    return std::unique_ptr<VertexArray>(new VertexArray(vao));
}

void VertexArray::Bind()
{
    GLCall(glBindVertexArray(_rendID));
}

void VertexArray::Unbind()
{
    GLCall(glBindVertexArray(0));
}

void VertexArray::AddBuffer(const VertexBuffer& vertexBuffer, const VertexBufferLayout& layout)
{
    Bind();
    vertexBuffer.Bind();

    auto& elements = layout.getElements();

    size_t offset = 0;

    for(unsigned int i = 0; i< elements.size(); i++)
    {
        auto& element = elements[i];
        GLCall(glVertexAttribPointer(i, element.count, element.type, element.normalized ? GL_TRUE : GL_FALSE, layout.getStride(), (void*)offset));
        GLCall(glEnableVertexAttribArray(i));

        offset += element.count * element.getTypeSize();
    }
}


template<typename T>
void push(unsigned int count)
{
    std::cout << "call wrong tempalte push method" << std::endl;
    assert(false);
}

template<>
void VertexBufferLayout::push<float>(unsigned int count)
{
    ElementLayout layout = {GL_FLOAT, count, false};
    _stride += layout.getTypeSize() * count;
    _elements.push_back(std::move(layout));
}

template<>
void VertexBufferLayout::push<unsigned int>(unsigned int count)
{
    ElementLayout layout = {GL_UNSIGNED_INT, count, false};
    _stride += layout.getTypeSize() * count;
    _elements.push_back(std::move(layout));
}

template<>
void VertexBufferLayout::push<unsigned char>(unsigned int count)
{
    ElementLayout layout = {GL_UNSIGNED_BYTE, count, true};
    _stride += layout.getTypeSize() * count;
    _elements.push_back(std::move(layout));
}

template void VertexBufferLayout::push<float>(unsigned int count);
template void VertexBufferLayout::push<unsigned int>(unsigned int count);
template void VertexBufferLayout::push<unsigned char>(unsigned int count);
