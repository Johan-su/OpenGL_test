#pragma once

#include "int.hpp"

class IndexBuffer
{
public:

    IndexBuffer(const U32 *data, Usize count, U32 usage);
    ~IndexBuffer();
    
    void bind();
    void unbind();



private:

    U32 buffer_id;
};