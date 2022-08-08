#pragma once
#include "int.hpp"


struct VertexBuffer
{
    U32 buffer_id;
};


struct IndexBuffer
{
    U32 count;
    U32 buffer_id;
};




struct LayoutElement
{
    U32 type;
    U32 count;
    U32 normalized;
};


struct LayoutArray
{
    Usize count;
    Usize max_size;
    LayoutElement elements[256];
};


struct VertexBufferLayout
{
    U32 m_stride;
    LayoutArray m_elements;
};


struct VertexArray
{
    U32 vao_id;
};



struct Texture
{
    U32 texture_id;
    const char *filepath;
    U8 *local_buffer;
    I32 width, height, BPP;
};


struct Shader
{
    U32 program_id;
};


Usize size_of_gltype(U32 type);


void init(VertexBuffer *vb, const void *data, Usize size, U32 usage);
void clean(VertexBuffer *vb);
void bind(VertexBuffer *vb);
void unbind(VertexBuffer *vb);




void init(IndexBuffer *ib, const U32 *data, Usize count, U32 usage);
void clean(IndexBuffer *ib);
void bind(IndexBuffer *ib);
void unbind(IndexBuffer *ib);




void init(LayoutArray *layout_array);
void add_element(LayoutArray *layout_array, LayoutElement element);


void init(VertexBufferLayout *layout);


/**
 * use the other push_type functions as this is internal
 */ 
void push_type_raw_gl_type(VertexBufferLayout *layout, U32 count, U32 normalized, U32 type);
void push_type_float(VertexBufferLayout *layout, U32 count, U32 normalized);
void push_type_U32(VertexBufferLayout *layout, U32 count, U32 normalized);
void push_type_U8(VertexBufferLayout *layout, U32 count, U32 normalized);




void init(VertexArray *va);
void clean(VertexArray *va);
void bind(VertexArray *va);
void unbind(VertexArray *va);
void add_buffer_to_array(VertexArray *va, VertexBuffer *vb, VertexBufferLayout *layout);




void init(Shader *shader, const char *vert_path, const char *frag_path);
void clean(Shader *shader);
void bind(Shader *shader);
void unbind(Shader *shader);

I32 get_uniform_location(Shader *shader, const char *name);
void set_uniform1i(Shader *shader, const char *name, int v0);
void set_uniform1f(Shader *shader, const char *name, float v0);
void set_uniform4f(Shader *shader, const char *name, float v0, float v1, float v2, float v3);



void init(Texture *texture, const char *filepath);
void clean(Texture *texture);
void bind(Texture *texture, U32 slot);
void unbind(Texture *texture);




