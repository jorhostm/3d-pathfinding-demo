#pragma once
#include <vector>
class Quad {
public:
    static const int FRONT = 0;
    static const int BACK = 1;
    static const int LEFT = 2;
    static const int RIGHT = 3;
    static const int TOP = 4;
    static const int BOTTOM = 5;

    static constexpr float frontFace[12] = { 0.0,1.0,0.0, 1.0,1.0,0.0, 1.0,0.0,0.0, 0.0,0.0,0.0 };
    static constexpr float backFace[12] = { 0.0,0.0,1.0, 1.0,0.0,1.0, 1.0,1.0,1.0, 0.0,1.0,1.0 };
    static constexpr float leftFace[12] = { 0.0,0.0,1.0, 0.0,1.0,1.0, 0.0,1.0,0.0, 0.0,0.0,0.0 };
    static constexpr float rightFace[12] = { 1.0,0.0,0.0, 1.0,1.0,0.0, 1.0,1.0,1.0, 1.0,0.0,1.0 };
    static constexpr float topFace[12] = { 0.0,1.0,1.0, 1.0,1.0,1.0, 1.0,1.0,0.0, 0.0,1.0,0.0 };
    static constexpr float bottomFace[12] = { 0.0,0.0,0.0, 1.0,0.0,0.0, 1.0,0.0,1.0, 0.0,0.0,1.0 };

    static constexpr float tex[12] = { 0.0,1.0,0.0, 1.0,1.0,0.0, 1.0,0.0,0.0, 0.0,0.0,0.0 };

    static void addQuad(std::vector<float>& meshdata, const int face, float position[], std::vector<float>& colour) {
        const float* quad;
        switch (face) {
        case FRONT:
            quad = frontFace;
            break;
        case BACK:
            quad = backFace;
            break;
        case LEFT:
            quad = leftFace;
            break;
        case RIGHT:
            quad = rightFace;
            break;
        case TOP:
            quad = topFace;
            break;
        case BOTTOM:
            quad = bottomFace;
            break;
        default:
            quad = frontFace;
        }
        for (int i = 0; i < 12; i += 3) {
            for (int j = 0; j < 3; j++) {
                meshdata.push_back(*(quad + i + j) + position[j]);
            }
            meshdata.insert(std::end(meshdata), std::begin(colour), std::end(colour));
            for (int j = 0; j < 2; j++) {
                meshdata.push_back(*(tex + i + j));
            }
        }

    }

};