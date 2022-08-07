#pragma once

#include "int.hpp"

class VertexBuffer
{
public:

    VertexBuffer(const void *data, Usize size, U32 usage);
    ~VertexBuffer();
    
    void bind();
    void unbind();



private:

    U32 buffer_id;
};