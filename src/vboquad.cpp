#include "vboquad.h"

#include "glad/glad.h"

VBOQuad::VBOQuad() {}
void VBOQuad::init()
{
    float vertices[4*2] = {
            1.f,  1.f,  // top right
            1.f, -1.f,  // bottom right
            -1.f, -1.f,  // bottom left
            -1.f,  1.f  // top left
    };
    float texCoord[4*3] = {
            1.f,  1.f, 0, // top right
            1.f, 0,  0, // bottom right
            0, 0, 0, // bottom left
            0,  1.f, 0  // top left
    };
    unsigned int indices[] = {
            0, 1, 3,
            1, 2, 3
    };
    glGenVertexArrays(1, &vaoHandle);
    glBindVertexArray(vaoHandle);

    unsigned int handle[3];
    glGenBuffers(3, handle);

    // position
    glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    // text coord
    glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texCoord), texCoord, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    // topology
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle[2]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void VBOQuad::render() {
    glBindVertexArray(vaoHandle);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
