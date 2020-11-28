#pragma once
#include "Chunk.h"
#include <unordered_map>
#include <iostream>
#include "VectorXYZ.h"

class World {
private:
    std::unordered_map<VectorXYZ, Chunk*, VectorHash> chunkMap = std::unordered_map<VectorXYZ, Chunk*, VectorHash>();

    Chunk* addChunk(VectorXYZ& c_pos);

    void removeChunk(VectorXYZ& c_pos);

public:

    World();

    void addBlock(int x, int y, int z);
    void addBlock(float x, float y, float z);

    void removeBlock(int x, int y, int z);
    void removeBlock(float x, float y, float z);
    Chunk* getChunk(int x, int y, int z);
    int getBlock(int x, int y, int z);
    int getBlock(float x, float y, float z);
    void draw();

    int modulo(int a, int b);
};