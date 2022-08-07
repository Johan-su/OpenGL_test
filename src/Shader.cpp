#define _CRT_SECURE_NO_WARNINGS
#include "Shader.hpp"

#include "../dependencies/GLFW/include/glfw3.h"
#include "../dependencies/Khronos/glext.h"
#include "LoadGL/load_gl.hpp"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

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

static U32 create_shader(const char *vertex_src, const char *fragment_src)
{
    U32 program = glCreateProgram();
    U32 vs = compile_shader(GL_VERTEX_SHADER, vertex_src);
    U32 fs = compile_shader(GL_FRAGMENT_SHADER, fragment_src);


    glAttachShader(program, vs); 
    glAttachShader(program, fs);

    glLinkProgram(program);
    glValidateProgram(program);

    glDetachShader(program, vs);
    glDetachShader(program, fs);


    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}


Shader::Shader(const char *vert_path, const char *frag_path)
{
    const char *vert_src = slurp_file(vert_path); 
    const char *frag_src = slurp_file(frag_path); 

    this->program_id = 0;
    this->program_id = create_shader(vert_src, frag_src);

    destroy_slurped_file(vert_src);
    destroy_slurped_file(frag_src);
}


Shader::~Shader()
{
    glDeleteProgram(this->program_id);
}


void Shader::bind()
{
    glUseProgram(this->program_id);
}


void Shader::unbind()
{
    glUseProgram(0);
}


void Shader::set_uniform4f(const char *name, float v0, float v1, float v2, float v3)
{
    I32 location = get_uniform_location(name); // TODO(Johan): cache or get uniform location when compiling shaders
    glUniform4f(location, v0, v1, v2, v3);
}


I32 Shader::get_uniform_location(const char *name)
{
    I32 location = glGetUniformLocation(this->program_id, name);
    if (location == -1)
        fprintf(stderr, "WARNING: uniform \"%s\" was not found\n", name);
    
    return location;
}



