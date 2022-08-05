#define _CRT_SECURE_NO_WARNINGS

#include "../dependencies/GLFW/include/glfw3.h"
#include "../dependencies/Khronos/glext.h"

#include "LoadGL/load_gl.hpp"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef signed char I8;
typedef short       I16;
typedef int         I32;
typedef long long   I64;


typedef unsigned char      U8;
typedef unsigned short     U16;
typedef unsigned int       U32;
typedef unsigned long long U64;

typedef unsigned long long Usize;





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


static void opengl_debug_callback(GLenum source,
            GLenum type,
            GLuint id,
            GLenum severity,
            GLsizei length,
            const GLchar *message,
            const void *userParam)
{
    if (type != GL_DEBUG_TYPE_ERROR)
        return;

    fprintf(stderr, "DEBUG OpenGL:\nSource: 0x%x\nType: 0x%x\n"
         "Id: 0x%x\nSeverity: 0x%x\n", source, type, id, severity);
    fprintf(stderr, "%s\n", message);
    exit(1);
}



static U32 parse_and_create_shader(const char *vertex_path, const char *fragment_path)
{
    const char *vert_src = slurp_file(vertex_path); 
    const char *frag_src = slurp_file(fragment_path); 


    U32 shader_program = create_shader(vert_src, frag_src);

    destroy_slurped_file(vert_src);
    destroy_slurped_file(frag_src);



    return shader_program;
}


int main(void) //TODO:(Johan) finish https://www.youtube.com/watch?v=bTHqmzjm2UI&list=PLlrATfBNZ98foTJPJ_Ev03o2oq3-GGOS2&index=13
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;


    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);


    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(800, 600, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glfwSwapInterval(1);

    if (load_gl() == 0)
    {
        fprintf(stderr, "ERROR: loading modern opengl functions failed\n");
        return -1;
    }
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(opengl_debug_callback, NULL);

    printf("%s\n", glGetString(GL_VERSION));

    float vertex_buffer[] = {
        -0.5f, -0.5f,
         0.5f, -0.5f,
         0.5f,  0.5f,
        -0.5f,  0.5f,
    };

    U32 indicies[] = {
        0, 1, 2,
        2, 3, 0,
    };


    U32 vao;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    U32 buffer_id;
    
    glGenBuffers(1, &buffer_id);  
    glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
    glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(float), vertex_buffer, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);


    U32 ibo;
    
    glGenBuffers(1, &ibo);  
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(U32), indicies, GL_STATIC_DRAW);


    U32 shader_program = parse_and_create_shader("assets/shaders/basic.vert", "assets/shaders/basic.frag");
    glUseProgram(shader_program);

    

    I32 location = glGetUniformLocation(shader_program, "u_Color");
    if (location == -1)
    {
        fprintf(stderr, "ERROR: failed to find uniform");
        exit(1);
    }


    float rgba[4] = {
        0.2f,
        0.4f,
        0.6f,
        1.0f,
    };
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {

        
        //rgba[0] = (float)(rand()) / RAND_MAX;
        //rgba[1] = (float)(rand()) / RAND_MAX;
        rgba[2] = (float)(rand()) / RAND_MAX;
        //rgba[3] = (float)(rand()) / RAND_MAX;

        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);


        glUseProgram(shader_program);
        glUniform4f(location, rgba[0], rgba[1], rgba[2], rgba[3]);

        glBindVertexArray(vao);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }


    glDeleteProgram(shader_program);

    glfwTerminate();
    return 0;
}