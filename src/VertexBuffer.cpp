#include "VertexBuffer.hpp"


#include "../dependencies/GLFW/include/glfw3.h"
#include "../dependencies/Khronos/glext.h"
#include "LoadGL/load_gl.hpp"


VertexBuffer::VertexBuffer(const void *data, Usize size, U32 usage)
{
    glGenBuffers(1, &this->buffer_id);
    glBindBuffer(GL_ARRAY_BUFFER, this->buffer_id); 
    glBufferData(GL_ARRAY_BUFFER, size, data, usage);
}


VertexBuffer::~VertexBuffer()
{
    glDeleteBuffers(1, &this->buffer_id);
}
    

void VertexBuffer::bind()
{
    glBindBuffer(GL_ARRAY_BUFFER, this->buffer_id);
}


void VertexBuffer::unbind()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

