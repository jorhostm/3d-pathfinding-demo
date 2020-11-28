#pragma once
#include <vector>
#include <iostream>
#include <algorithm>
#include <GL/glew.h>
#include "Utils.h"
//static void create_vbo(std::vector<float>& vertices, unsigned int* vao, unsigned int* vbo) {
//
//    unsigned int array_id = 0;
//    glGenVertexArrays(1, &array_id);
//    glBindVertexArray(array_id);
//
//    unsigned int buffer_id = 0;
//    std::cout << buffer_id << " ";
//    glGenBuffers(1, &buffer_id);
//    std::cout << buffer_id << std::endl;
//    glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
//    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertices[0]), &vertices[0], GL_DYNAMIC_DRAW);
//
//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 36, NULL);
//    glEnableVertexAttribArray(0);
//    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 36, (const void*)12);
//    glEnableVertexAttribArray(1);
//    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 36, (const void*)28);
//    glEnableVertexAttribArray(2);
//
//    *vao = array_id;
//    *vbo = buffer_id;
//}



class Chunk {
public:
    static const int CHUNK_SIZE = 16;
private:
    int arr[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE] = { 0 };

    std::vector<float> meshdata = std::vector<float>(0);

    int count = 0;

    bool updated = true;

    unsigned int vao = 0;
    unsigned int vbo = 0;

    float pos[3] = { 0,0,0 };

    std::vector<float> red = { 1.0,0.0,0.0,1.0 };
    std::vector<float> green = { 0.0,1.0,0.0,1.0 };
    std::vector<float> blue = { 0.0,0.0,1.0,1.0 };

public:


    Chunk(const int x, const int y, const int z);

    ~Chunk();

    unsigned int getVBO();

    int getCount();

    std::vector<float> getMeshdata();
    void clearMeshdata();
    void render();
    void createMesh();
    void addBlock(int x, int y, int z);
    void removeBlock(int x, int y, int z);
    int getBlock(int x, int y, int z);
};



