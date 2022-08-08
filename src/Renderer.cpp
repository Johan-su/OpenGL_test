#include "Renderer.hpp"

#include "LoadGL/load_gl.hpp"


void Renderer::draw(VertexArray *va, IndexBuffer *ib, Shader *shader)
{
    bind(shader);
    bind(va);
    bind(ib);

    glDrawElements(GL_TRIANGLES, ib->count, GL_UNSIGNED_INT, NULL);
}


void Renderer::clear()
{
    glClear(GL_COLOR_BUFFER_BIT);
}