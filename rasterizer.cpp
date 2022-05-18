#include "rasterizer.hpp"
#include "BMP.h"

Rasterizer::Rasterizer(BMP &buffer) : mBuffer(buffer) {

}

void Rasterizer::drawTriangle(float x1, float y1, float z1, const float3& normal1, float x2, float y2, float z2, const float3& normal2, float x3, float y3, float z3, const float3& normal3, const Light& light, const std::vector<float3>& positions, const Vertex& f1, const Vertex& f2, const Vertex& f3) {
    mBuffer.fill_triangle(toPixelX(x1), toPixelY(y1), z1, normal1, toPixelX(x2), toPixelY(y2), z2, normal2, toPixelX(x3), toPixelY(y3), z3, normal3, light, positions, f1, f2, f3);
}

void Rasterizer::drawTriangleVertex(float x1, float y1, float z1, const float3& vertexColors1, float x2, float y2, float z2, const float3& vertexColors2, float x3, float y3, float z3, const float3& vertexColors3) {
    mBuffer.fill_triangle_vertex(toPixelX(x1), toPixelY(y1), z1, vertexColors1, toPixelX(x2), toPixelY(y2), z2, vertexColors2, toPixelX(x3), toPixelY(y3), z3, vertexColors3);
}


int Rasterizer::toPixelX(float x) const {
    return (x+1)*mBuffer.bmp_info_header.width *0.5f;
}

int Rasterizer::toPixelY(float y) const {
    // BMP format requires reverting y axis
    return mBuffer.bmp_info_header.height - ((y+1)*mBuffer.bmp_info_header.height *0.5f);
}
