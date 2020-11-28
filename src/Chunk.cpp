#include "Chunk.h"
#include "Quad.h"

    Chunk::Chunk(const int x, const int y, const int z) {
        pos[0] = x;
        pos[1] = y;
        pos[2] = z;
        std::cout << "Created chunk at " << pos[0] << " " << pos[1] << " " << pos[2] << std::endl;
    };

    Chunk::~Chunk() {
        std::cout << "Deleting chunk at " << pos[0] << " " << pos[1] << " " << pos[2] << std::endl;
        clearMeshdata();
        glDeleteBuffers(1, &vbo);
        glDeleteVertexArrays(1, &vao);
    };

    unsigned int Chunk::getVBO() {
        return vbo;
    }

    int Chunk::getCount() {
        return count;
    }

    std::vector<float> Chunk::getMeshdata() {
        return meshdata;
    }

    void Chunk::clearMeshdata() {
        meshdata.clear();
    }

    void Chunk::render() {
        if (updated) {
            createMesh();
        }
        glBindVertexArray(vao);
        glDrawArrays(GL_QUADS, 0, count);
    }

    void Chunk::createMesh() {
        count = 0;
        for (int x = 0; x < CHUNK_SIZE; x++) {
            for (int y = 0; y < CHUNK_SIZE; y++) {
                for (int z = 0; z < CHUNK_SIZE; z++) {
                    if (arr[x][y][z] != 0) {
                        float blockPos[3] = { pos[0] + x,pos[1] + y, pos[2] + z };

                        if (!getBlock(x, y, z - 1)) {
                            Quad::addQuad(meshdata, Quad::FRONT, blockPos, red);
                            count += 4;
                        }
                        if (!getBlock(x, y, z + 1)) {
                            Quad::addQuad(meshdata, Quad::BACK, blockPos, red);
                            count += 4;
                        }
                        if (!getBlock(x - 1, y, z)) {
                            Quad::addQuad(meshdata, Quad::LEFT, blockPos, green);
                            count += 4;
                        }
                        if (!getBlock(x + 1, y, z)) {
                            Quad::addQuad(meshdata, Quad::RIGHT, blockPos, green);
                            count += 4;
                        }
                        if (!getBlock(x, y + 1, z)) {
                            Quad::addQuad(meshdata, Quad::TOP, blockPos, blue);
                            count += 4;
                        }
                        if (!getBlock(x, y - 1, z)) {
                            Quad::addQuad(meshdata, Quad::BOTTOM, blockPos, blue);
                            count += 4;
                        }

                    }
                }
            }
        }
        if (!vao) {
            Utils::create_vbo(meshdata, &vao, &vbo);
        }
        else {
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, meshdata.size() * sizeof(meshdata[0]), &meshdata[0], GL_DYNAMIC_DRAW);
        }
        clearMeshdata();
        updated = false;
    }

    void Chunk::addBlock(int x, int y, int z) {
        if (!arr[x][y][z]) {
            arr[x][y][z] = 1;
            updated = true;
        }
    }
    void Chunk::removeBlock(int x, int y, int z) {
        if (arr[x][y][z]) {
            arr[x][y][z] = 0;
            updated = true;
        }
    }
    int Chunk::getBlock(int x, int y, int z) {
        if (x >= 0 && x < CHUNK_SIZE && y >= 0 && y < CHUNK_SIZE && z >= 0 && z < CHUNK_SIZE) {
            return arr[x][y][z];
        }

        return 0;
    }