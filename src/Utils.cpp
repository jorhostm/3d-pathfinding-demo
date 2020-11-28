#include "Utils.h"
#include <GL/glew.h>

void Utils::create_vbo(std::vector<float>& vertices, unsigned int* vao, unsigned int* vbo) {

    unsigned int array_id = 0;
    glGenVertexArrays(1, &array_id);
    glBindVertexArray(array_id);

    unsigned int buffer_id = 0;
    glGenBuffers(1, &buffer_id);
    glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertices[0]), &vertices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 36, NULL);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 36, (const void*)12);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 36, (const void*)28);
    glEnableVertexAttribArray(2);

    *vao = array_id;
    *vbo = buffer_id;
}