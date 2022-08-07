#include "IndexBuffer.hpp"

#include "../dependencies/GLFW/include/glfw3.h"
#include "../dependencies/Khronos/glext.h"
#include "LoadGL/load_gl.hpp"


IndexBuffer::IndexBuffer(const U32 *data, Usize count, U32 usage)
{
    glGenBuffers(1, &this->buffer_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->buffer_id); 
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(U32), data, usage);
}


IndexBuffer::~IndexBuffer()
{
    glDeleteBuffers(1, &this->buffer_id);
}
    

void IndexBuffer::bind()
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->buffer_id);
}


void IndexBuffer::unbind()
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}



