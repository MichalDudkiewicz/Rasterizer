#include "mesh.hpp"
#include <utility>
#include <algorithm>
#include "vertex.hpp"

void Mesh::drawVertex(Rasterizer &rasterizer, VertexProcessor &vertexProcessor, Light& light) {
    calculateNormals();
    for (const auto& triangle : mIndices)
    {
        std::vector<float3> positions;
        std::vector<float3> normals;
        positions.reserve(triangle.size());
        for (int i = 0; i < triangle.size(); i++)
        {
            positions.push_back(vertexProcessor.convertToCanonical(mVertices[triangle[i]].position));
            // TODO: these normals should also be converted but only to View (not projection)
            normals.push_back(mVertices[triangle[i]].normal);
        }
        Vertex fragment1;
        fragment1.position = positions[0];
        fragment1.normal = normals[0];
        const auto vertexColors1 = light.calculate(fragment1, vertexProcessor);
        Vertex fragment2;
        fragment2.position = positions[1];
        fragment2.normal = normals[1];
        const auto vertexColors2 = light.calculate(fragment2, vertexProcessor);
        Vertex fragment3;
        fragment3.position = positions[2];
        fragment3.normal = normals[2];
        const auto vertexColors3 = light.calculate(fragment3, vertexProcessor);
        rasterizer.drawTriangleVertex(positions[0].x(), positions[0].y(), positions[0].z(), vertexColors1, positions[1].x(), positions[1].y(), positions[1].z(), vertexColors2, positions[2].x(), positions[2].y(), positions[2].z(), vertexColors3);
    }
}

void Mesh::draw(Rasterizer &rasterizer, VertexProcessor &vertexProcessor, Light& light) {
    calculateNormals();
    for (const auto& triangle : mIndices)
    {
        std::vector<float3> positions;
        std::vector<float3> normals;
        positions.reserve(triangle.size());
        for (int i = 0; i < triangle.size(); i++)
        {
            positions.push_back(vertexProcessor.convertToCanonical(mVertices[triangle[i]].position));
            normals.push_back(mVertices[triangle[i]].normal);
        }
        Vertex fragment1;
        fragment1.position = positions[0];
        fragment1.normal = normals[0];
        Vertex fragment2;
        fragment2.position = positions[1];
        fragment2.normal = normals[1];
        Vertex fragment3;
        fragment3.position = positions[2];
        fragment3.normal = normals[2];

        const float Su = 1.0f;
        const float Sv = 1.0f;
        // planar texturing
//        const float v1 = std::clamp(Sz * fragment1.position.x() - mCenter.position.x(), 0.0f, 1.0f);
//        const float v2 = std::clamp(Sz * fragment2.position.x() - mCenter.position.x(), 0.0f, 1.0f);
//        const float v3 = std::clamp(Sz * fragment3.position.x() - mCenter.position.x(), 0.0f, 1.0f);
//        const float u1 = std::clamp(Sx * fragment1.position.y() - mCenter.position.y(), 0.0f, 1.0f);
//        const float u2 = std::clamp(Sx * fragment2.position.y() - mCenter.position.y(), 0.0f, 1.0f);
//        const float u3 = std::clamp(Sx * fragment3.position.y() - mCenter.position.y(), 0.0f, 1.0f);
//        fragment1.textureCoords = float3{u1, v1, 0};
//        fragment2.textureCoords = float3{u2, v2, 0};
//        fragment3.textureCoords = float3{u3, v3, 0};

        const float len1 = sqrtf(powf(fragment1.position.x(), 2) + powf(fragment1.position.y(), 2) + powf(fragment1.position.z(), 2));
        const float len2 = sqrtf(powf(fragment2.position.x(), 2) + powf(fragment2.position.y(), 2) + powf(fragment2.position.z(), 2));
        const float len3 = sqrtf(powf(fragment3.position.x(), 2) + powf(fragment3.position.y(), 2) + powf(fragment3.position.z(), 2));

        const float xTextureCenter = 0.5f;
//        const float v1 = std::clamp(Sv/(M_PIf32)* tanf(fragment1.position.y()/len1) - xTextureCenter, 0.0f, 1.0f);
//        const float v2 = std::clamp(Sv/(M_PIf32)* tanf(fragment2.position.y()/len2) - xTextureCenter, 0.0f, 1.0f);
//        const float v3 = std::clamp(Sv/(M_PIf32)* tanf(fragment3.position.y()/len3) - xTextureCenter, 0.0f, 1.0f);
//
//        const float u1 = std::clamp(Su/(2*M_PIf32)* atan2f(fragment1.position.z(),fragment1.position.y()) - xTextureCenter, 0.0f, 1.0f);
//        const float u2 = std::clamp(Su/(2*M_PIf32)* atan2f(fragment2.position.z(),fragment2.position.y()) - xTextureCenter, 0.0f, 1.0f);
//        const float u3 = std::clamp(Su/(2*M_PIf32)* atan2f(fragment3.position.z(),fragment3.position.y()) - xTextureCenter, 0.0f, 1.0f);

        const float v1 = std::clamp(Sv * asinf(fragment1.position.y()/M_PIf32) + xTextureCenter, 0.0f, 1.0f);
        const float v2 = std::clamp(Sv * asinf(fragment2.position.y()/M_PIf32) + xTextureCenter, 0.0f, 1.0f);
        const float v3 = std::clamp(Sv * asinf(fragment3.position.y()/M_PIf32) + xTextureCenter, 0.0f, 1.0f);

        const float u1 = std::clamp(Su/(2*M_PIf32)* atan2f(fragment1.position.x(),fragment1.position.z()) + xTextureCenter, 0.0f, 1.0f);
        const float u2 = std::clamp(Su/(2*M_PIf32)* atan2f(fragment2.position.x(),fragment2.position.z()) + xTextureCenter, 0.0f, 1.0f);
        const float u3 = std::clamp(Su/(2*M_PIf32)* atan2f(fragment3.position.x(),fragment3.position.z()) + xTextureCenter, 0.0f, 1.0f);

        fragment1.textureCoords = float3{u1, v1, 0};
        fragment2.textureCoords = float3{u2, v2, 0};
        fragment3.textureCoords = float3{u3, v3, 0};


        rasterizer.drawTriangle(positions[0].x(), positions[0].y(), positions[0].z(), normals[0], positions[1].x(), positions[1].y(), positions[1].z(), normals[1], positions[2].x(), positions[2].y(), positions[2].z(), normals[2], light, positions, fragment1, fragment2, fragment3);
    }
}

Mesh::Mesh(int vSize, int tSize, Vertex center) : mVertices(vSize), mIndices(tSize), mCenter(std::move(center)) {
}

void Mesh::calculateNormals() {
    for (auto & mVertice : mVertices)
        mVertice.normal = float3{0.0f, 0.0f, 0.0f};
    for (const auto & mIndice : mIndices)
    {
        float3 n = crossProduct((mVertices[mIndice.z()].position -
                               mVertices[mIndice.x()].position), mVertices[mIndice.y()].position - mVertices[ mIndice.x()].position);
        n.normalize();
        mVertices [ mIndice.x() ].normal += n ;
        mVertices [ mIndice.y() ].normal += n ;
        mVertices [ mIndice.z() ].normal += n ;
    }
    for (auto & mVertice : mVertices)
    {
        mVertice.normal.normalize();
    }
}

