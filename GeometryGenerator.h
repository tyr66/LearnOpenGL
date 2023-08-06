#pragma once

#include <vector>
#include <memory>

class Model;

class GeometryGenerator {
public:
    static void generateCube(std::vector<float>& verts, std::vector<unsigned int>& indices, float w, float h , float d);
    static void generateSphere(std::vector<float>& verts, std::vector<unsigned int >&indices, float r);
    static std::unique_ptr<Model> generateWindowQuad();
};

