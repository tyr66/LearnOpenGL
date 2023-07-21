#include <cmath>
#include <cassert>
#include "GeometryGenerator.h"



void GeometryGenerator::generateCube(std::vector<float>& verts, std::vector<unsigned int>& indices, float w, float h , float d)
{
    verts.clear();
    verts = {
        // 前面
        -0.5, 0.5, -0.5, 0.0f, 0.0f, 1.0f,   
        0.5, 0.5, -0.5,  0.0f, 0.0f, 1.0f,
        0.5, -0.5, -0.5,  0.0f, 0.0f, 1.0f,
        -0.5, -0.5, -0.5,  0.0f, 0.0f, 1.0f,

        // 后面
        -0.5, 0.5, 0.5, 0.0f, 0.0f, -1.0f,
        0.5, 0.5, 0.5, 0.0f, 0.0f, -1.0f,
        0.5, -0.5, 0.5, 0.0f, 0.0f, -1.0f,
        -0.5, -0.5, 0.5, 0.0f, 0.0f, -1.0f,   

        // 左面
        -0.5, 0.5, -0.5, -1.0f, 0.0f, 0.0f,
        -0.5, 0.5, 0.5, -1.0f, 0.0f, 0.0f,
        -0.5, -0.5, 0.5, -1.0f, 0.0f, 0.0f,
        -0.5, -0.5, -0.5, -1.0f, 0.0f, 0.0f,

        // 右面
        0.5, 0.5, -0.5, 1.0f, 0.0f, 0.0f,
        0.5, 0.5, 0.5, 1.0f, 0.0f, 0.0f,
        0.5, -0.5, 0.5, 1.0f, 0.0f, 0.0f,
        0.5, -0.5, -0.5, 1.0f, 0.0f, 0.0f, 

        // 上面
        -0.5, 0.5, -0.5, 0.0f, 1.0f, 0.0f,
        -0.5, 0.5, 0.5, 0.0f, 1.0f, 0.0f,
        0.5, 0.5, 0.5, 0.0f, 1.0f, 0.0f,
        0.5, 0.5, -0.5, 0.0f, 1.0f, 0.0f, 

        // 下面
        -0.5, -0.5, -0.5, 0.0f, -1.0f, 0.0f,
        -0.5, -0.5, 0.5, 0.0f, -1.0f, 0.0f,
        0.5, -0.5, 0.5, 0.0f, -1.0f, 0.0f,
        0.5, -0.5, -0.5, 0.0f, -1.0f, 0.0f,
    };

    for (int i = 0 ; i< verts.size(); i+=3)
    {
        verts[i] *= w;
        verts[i + 1] *= h;
        verts[i + 2] *= d;
    }

    indices.clear();
    indices = {
        //,前面
        0,2,1,
        2,0,3,

        //,后面
        4,6,7,
        6,4,5,

        //,左面
        8,10,11,
        10,8,9,

        //,右面
        12,14,15,
        14,12,13,

        //,上面
        16,18,19,
        18,16,17,

        //,下面
        20,22,23,
        22,20,21,
    };
}
void GeometryGenerator::generateSphere(std::vector<float>& verts, std::vector<unsigned int >&indices, float r)
{

}