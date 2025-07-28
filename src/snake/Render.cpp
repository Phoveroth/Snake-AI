#include "Render.h"

void Render::Draw(const VertexArray &va, const IndexBuffer &ib, const Shader &shader)
{
    shader.Bind();
    va.Bind();
    ib.Bind();
    GLCall(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
}

void Render::DrawInstanced(const VertexArray &va, const IndexBuffer &ib, const Shader &shader, GLsizei count)
{
    shader.Bind();
    va.Bind();
    ib.Bind();
    GLCall(glDrawElementsInstanced(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr, count));
}

void Render::DrawLines(const VertexArray &va, const IndexBuffer &ib, const Shader &shader)
{
    shader.Bind();
    va.Bind();
    ib.Bind();
    GLCall(glDrawElements(GL_LINES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
}
