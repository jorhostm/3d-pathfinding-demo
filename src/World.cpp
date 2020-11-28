#include "World.h"
#include <cmath>

    Chunk* World::addChunk(VectorXYZ& c_pos) {
        Chunk* c = new Chunk(c_pos.x, c_pos.y, c_pos.z);
        chunkMap[c_pos] = c;
        return c;
    }

    void World::removeChunk(VectorXYZ& c_pos) {
        delete chunkMap[c_pos];
        chunkMap.erase(c_pos);
    }



    World::World() = default;

    void World::addBlock(int x, int y, int z) {
        int dx = modulo(x, Chunk::CHUNK_SIZE);
        int dy = modulo(y, Chunk::CHUNK_SIZE);
        int dz = modulo(z, Chunk::CHUNK_SIZE);
        VectorXYZ c_pos = VectorXYZ(x - dx, y - dy, z - dz);
        Chunk* c = chunkMap[c_pos];
        if (!c) {
            c = addChunk(c_pos);
        }
        if (!c->getBlock(dx, dy, dz)) {
            std::cout << "Map size " << chunkMap.size() << " Adding block at " << dx << " " << dy << " " << dz << std::endl;
            c->addBlock(dx, dy, dz);
        }
    }

    void World::removeBlock(int x, int y, int z) {
        int dx = modulo(x, Chunk::CHUNK_SIZE);
        int dy = modulo(y, Chunk::CHUNK_SIZE);
        int dz = modulo(z, Chunk::CHUNK_SIZE);
        VectorXYZ c_pos = VectorXYZ(x - dx, y - dy, z - dz);
        Chunk* c = chunkMap[c_pos];
        if (c) {
            if (c->getBlock(dx, dy, dz)) {
                std::cout << "Map size " << chunkMap.size() << std::endl;
                if (c->getCount() == 24) {
                    removeChunk(c_pos);
                }
                else {
                    c->removeBlock(dx, dy, dz);
                }
            }
        }


    }
    Chunk* World::getChunk(int x, int y, int z) {
        int dx = modulo(x, Chunk::CHUNK_SIZE);
        int dy = modulo(y, Chunk::CHUNK_SIZE);
        int dz = modulo(z, Chunk::CHUNK_SIZE);
        VectorXYZ c_pos = VectorXYZ(x - dx, y - dy, z - dz);
        Chunk* c = chunkMap[c_pos];
        return c;
    }
    int World::getBlock(int x, int y, int z) {
        Chunk* c = getChunk(x, y, z);
        if (c) {
            int dx = modulo(x, Chunk::CHUNK_SIZE);
            int dy = modulo(y, Chunk::CHUNK_SIZE);
            int dz = modulo(z, Chunk::CHUNK_SIZE);
            return c->getBlock(dx, dy, dz);
        }
        else {
            return 0;
        }
    }
    void World::draw() {
        for (auto& it : chunkMap) {
            if (it.second)
                it.second->render();
        }
    }

    int World::getBlock(float x, float y, float z)
    {
        return getBlock((int) std::floor(x), (int)std::floor(y), (int)std::floor(z));
    }

    int World::modulo(int a, int b) {
        return a & (b - 1);
    }

    void World::addBlock(float x, float y, float z)
    {
        addBlock((int)std::floor(x), (int)std::floor(y), (int)std::floor(z));
    }

    void World::removeBlock(float x, float y, float z)
    {
        removeBlock((int)std::floor(x), (int)std::floor(y), (int)std::floor(z));
    }
