#define _CRT_SECURE_NO_WARNINGS
#include "render_buffers.hpp"

#include "LoadGL/load_gl.hpp"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "../dependencies/stb_image/stb_image.h"

Usize size_of_gltype(U32 type)
{
    switch (type)
    {
        case GL_FLOAT: return sizeof(GLfloat);
        case GL_UNSIGNED_INT: return sizeof(GLuint);
        case GL_UNSIGNED_BYTE: return sizeof(GLubyte); 

        default:
        {
            fprintf(stderr, "ERROR: type not found\n");
            return 0;
        } break;
    }    
}


void init(VertexBuffer *vb, const void *data, Usize size, U32 usage)
{
    glGenBuffers(1, &vb->buffer_id);
    glBindBuffer(GL_ARRAY_BUFFER, vb->buffer_id); 
    glBufferData(GL_ARRAY_BUFFER, size, data, usage);
}


void clean(VertexBuffer *vb)
{
    glDeleteBuffers(1, &vb->buffer_id);
}


void bind(VertexBuffer *vb)
{
    glBindBuffer(GL_ARRAY_BUFFER, vb->buffer_id);
}


void unbind(VertexBuffer *vb)
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}




void init(IndexBuffer *ib, const U32 *data, Usize count, U32 usage)
{
    ib->count = count;
    glGenBuffers(1, &ib->buffer_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib->buffer_id); 
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(U32), data, usage);
}


void clean(IndexBuffer *ib)
{
    glDeleteBuffers(1, &ib->buffer_id);
}


void bind(IndexBuffer *ib)
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib->buffer_id);
}


void unbind(IndexBuffer *ib)
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}



void init(LayoutArray *layout_array)
{
    layout_array->count = 0;
    layout_array->max_size = 256;
}


void add_element(LayoutArray *layout_array, LayoutElement element)
{
    if (layout_array->count < layout_array->max_size)
    {
        layout_array->elements[layout_array->count] = element;
        layout_array->count += 1;
    }
    else
    {
        fprintf(stderr, "WARNING: layout array full");
    }
}






void init(VertexBufferLayout *layout)
{
    layout->m_stride = 0;
    init(&layout->m_elements);
}


void push_type_raw_gl_type(VertexBufferLayout *layout, U32 count, U32 normalized, U32 type)
{
    LayoutElement le = {
        type,
        count,
        normalized,
    };
    add_element(&layout->m_elements, le);
    layout->m_stride += size_of_gltype(type) * count; 
}


void push_type_float(VertexBufferLayout *layout, U32 count, U32 normalized)
{ 
    push_type_raw_gl_type(layout, count, normalized, GL_FLOAT);
}


void push_type_U32(VertexBufferLayout *layout, U32 count, U32 normalized)
{
    push_type_raw_gl_type(layout, count, normalized, GL_UNSIGNED_INT);
}


void push_type_U8(VertexBufferLayout *layout, U32 count, U32 normalized)
{
    push_type_raw_gl_type(layout, count, normalized, GL_UNSIGNED_BYTE);
}

















void init(VertexArray *va)
{
    glGenVertexArrays(1, &va->vao_id);
}


void clean(VertexArray *va)
{
    glDeleteVertexArrays(1, &va->vao_id);
}


void bind(VertexArray *va)
{
    glBindVertexArray(va->vao_id);
}


void unbind(VertexArray *va)
{
    glBindVertexArray(0);
}


void add_buffer_to_array(VertexArray *va, VertexBuffer *vb, VertexBufferLayout *layout)
{
    bind(va);
    bind(vb);

    U32 offset = 0;

    for (Usize i = 0; i < layout->m_elements.count; ++i)
    {
        LayoutElement element = layout->m_elements.elements[i];

        glEnableVertexAttribArray(i);
        glVertexAttribPointer(i, 
            element.count, 
            element.type, 
            element.normalized, 
            layout->m_stride, 
            (const void *)(U64)offset);
        offset += element.count * size_of_gltype(element.type);
    }


}




static char *slurp_file(const char *file_path)
{
    FILE *file = fopen(file_path, "r");

    if (file == NULL)
    {
        fprintf(stderr, "ERROR: failed to read file with %d\n", errno);
        exit(1);
    }

    fseek(file, 0, SEEK_END);

    I64 file_length = ftell(file);

    rewind(file);

    char *slurped_file = (char *)malloc(file_length + 1);    
    memset(slurped_file, 0, file_length + 1);

    fread(slurped_file, 1, file_length, file);


    fclose(file);
    return slurped_file;
}


static void destroy_slurped_file(const char *slurped_file)
{
    free((void *)slurped_file);
}



static U32 compile_shader(U32 type, const char *source)
{
    U32 shader_id = glCreateShader(type);
    glShaderSource(shader_id, 1, &source, NULL);
    glCompileShader(shader_id);


    I32 compiled;

    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compiled);

    if (compiled == GL_FALSE)
    {
        I32 log_length = 0;

        char message[2048];


        glGetShaderInfoLog(shader_id, 2048, &log_length, message);

        const char *type_s;

        switch (type)
        {
            case GL_VERTEX_SHADER:
            {
                type_s = "Vertex shader";
            } break;
            

            case GL_FRAGMENT_SHADER:
            {
                type_s = "Fragment shader";
            } break;

            
            default:
            {
                type_s = "Unknown shader";
            } break;
        }


        fprintf(stderr, "ERROR: failed to compile %s with \n%s", type_s, message);
        glDeleteShader(shader_id);
        exit(1);
    }
    return shader_id;
}


void init(Shader *shader, const char *vert_path, const char *frag_path)
{
    const char *vert_src = slurp_file(vert_path); 
    const char *frag_src = slurp_file(frag_path); 

    shader->program_id = glCreateProgram();

    U32 vs = compile_shader(GL_VERTEX_SHADER, vert_src);
    U32 fs = compile_shader(GL_FRAGMENT_SHADER, frag_src);


    glAttachShader(shader->program_id, vs); 
    glAttachShader(shader->program_id, fs);

    glLinkProgram(shader->program_id);
    glValidateProgram(shader->program_id);

    glDetachShader(shader->program_id, vs);
    glDetachShader(shader->program_id, fs);


    glDeleteShader(vs);
    glDeleteShader(fs);


    destroy_slurped_file(vert_src);
    destroy_slurped_file(frag_src);
}


void clean(Shader *shader)
{
    glDeleteProgram(shader->program_id);
}


void bind(Shader *shader)
{
    glUseProgram(shader->program_id);
}


void unbind(Shader *shader)
{
    glUseProgram(0);
}


I32 get_uniform_location(Shader *shader, const char *name)
{
    I32 location = glGetUniformLocation(shader->program_id, name);
    if (location == -1)
        fprintf(stderr, "WARNING: uniform \"%s\" was not found\n", name);
    
    return location;
}


void set_uniform1i(Shader *shader, const char *name, int v0)
{
    I32 location = get_uniform_location(shader, name);
    glUniform1i(location, v0);
}


void set_uniform1f(Shader *shader, const char *name, float v0)
{
    I32 location = get_uniform_location(shader, name);
    glUniform1f(location, v0);
}


void set_uniform4f(Shader *shader, const char *name, float v0, float v1, float v2, float v3)
{
    I32 location = get_uniform_location(shader, name);
    glUniform4f(location, v0, v1, v2, v3);
}










void init(Texture *texture, const char *filepath)
{
    texture->texture_id = 0;
    texture->filepath = filepath;
    texture->local_buffer = NULL;
    texture->width = 0;
    texture->height = 0;
    texture->BPP = 0;

    stbi_set_flip_vertically_on_load(1);

    texture->local_buffer = stbi_load(filepath, &texture->width, &texture->height, &texture->BPP, 4);

    glGenTextures(1, &texture->texture_id);
    glBindTexture(GL_TEXTURE_2D, texture->texture_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, texture->width, texture->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture->local_buffer);
    glBindTexture(GL_TEXTURE_2D, 0);

    if (texture->local_buffer)
        stbi_image_free(texture->local_buffer);

}


void clean(Texture *texture)
{
    glDeleteTextures(1, &texture->texture_id); 
}


void bind(Texture *texture, U32 slot)
{
    glActiveTexture(GL_TEXTURE0 + slot); 
    glBindTexture(GL_TEXTURE_2D, texture->texture_id);
}


void unbind(Texture *texture)
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

