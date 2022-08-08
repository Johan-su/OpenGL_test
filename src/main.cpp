
#include "../dependencies/GLFW/include/glfw3.h"
#include "../dependencies/Khronos/glext.h"

#include "LoadGL/load_gl.hpp"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "int.hpp"

#include "Renderer.hpp"

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


int main(void) //TODO:(Johan) finish
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
        fprintf(stderr, "ERROR: loading modern OpenGL functions failed\n");
        return -1;
    }
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(opengl_debug_callback, NULL);

    printf("%s\n", glGetString(GL_VERSION));

    float vertex_data[] = {
        -0.5f, -0.5f, 0.0f, 0.0f,
         0.5f, -0.5f, 1.0f, 0.0f,
         0.5f,  0.5f, 1.0f, 1.0f,
        -0.5f,  0.5f, 0.0f, 1.0f,
    };

    U32 indicies[] = {
        0, 1, 2,
        2, 3, 0,
    };


    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    VertexArray va;

    init(&va);
    bind(&va);



    VertexBuffer vb;

    init(&vb, vertex_data, 4 * 4 * sizeof(float), GL_STATIC_DRAW);

    VertexBufferLayout layout;

    init(&layout);


    push_type_float(&layout, 2, GL_FALSE);
    push_type_float(&layout, 2, GL_FALSE);

    add_buffer_to_array(&va, &vb, &layout);



    IndexBuffer ib;
    init(&ib, indicies, 6, GL_STATIC_DRAW);

    Shader shader;

    init(&shader, "assets/shaders/basic.vert", "assets/shaders/basic.frag");

    Texture texture;
    init(&texture, "assets/images/placeholder.png");
    int slot = 0;

    bind(&texture, slot);
    bind(&shader);

    set_uniform1i(&shader, "u_Texture", slot);


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
        Renderer::clear();



        bind(&shader);
        set_uniform4f(&shader, "u_Color", rgba[0], rgba[1], rgba[2], rgba[3]);

        Renderer::draw(&va, &ib, &shader);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }


    clean(&va);
    clean(&vb);
    clean(&ib);
    clean(&shader);
    clean(&texture);

    glfwTerminate();
    return 0;
}