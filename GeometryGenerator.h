#pragma once

#include <vector>
#include <memory>
#include <string>

#include "Mesh.h"

class Model;

class GeometryGenerator {
public:
    static MeshPtr generateQuad();
    static MeshPtr generateCube();
    static MeshPtr generateInverseCube();
    static MeshPtr generatePlane();
    static MeshPtr generateSkyBox();
};

