#pragma once

#include "int.hpp"

class Shader
{
public:

    Shader(const char *vert_path, const char *frag_path);
    ~Shader();

    void bind();
    void unbind();

    void set_uniform4f(const char *name, float v0, float v1, float v2, float v3);
private:

    I32 get_uniform_location(const char *name);


private:
    U32 program_id;
};