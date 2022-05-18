#pragma once

#include "BMP.h"
#include "vertex.hpp"
#include "vector.hpp"
#include "light.hpp"

class Rasterizer {
public:
    explicit Rasterizer(BMP& buffer);

    /*
     * draw triangle clockwise using canonical space
     */
    void drawTriangle(float x1, float y1, float z1, const float3& vertexColors1, float x2, float y2, float z2, const float3& vertexColors2, float x3, float y3, float z3, const float3& vertexColors3, const Light& light, const std::vector<float3>& positions, const Vertex& f1, const Vertex& f2, const Vertex& f3);

    void drawTriangleVertex(float x1, float y1, float z1, const float3& vertexColors1, float x2, float y2, float z2, const float3& vertexColors2, float x3, float y3, float z3, const float3& vertexColors3);

private:
    int toPixelX(float x) const;

    int toPixelY(float y) const;

private:
    BMP& mBuffer;
};