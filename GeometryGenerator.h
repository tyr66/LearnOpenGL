#pragma once

#include <vector>

class GeometryGenerator {
public:
    static void generateCube(std::vector<float>& verts, std::vector<unsigned int>& indices, float w, float h , float d);
    static void generateSphere(std::vector<float>& verts, std::vector<unsigned int >&indices, float r);
};

