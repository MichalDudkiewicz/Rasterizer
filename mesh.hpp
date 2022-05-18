#pragma once

#include "vector.hpp"
#include "rasterizer.hpp"
#include "vertex_processor.hpp"
#include "vertex.hpp"
#include "light.hpp"

class Mesh {
public:
    Mesh(int vSize, int tSize, Vertex center);

public:
    void draw(Rasterizer& rasterizer, VertexProcessor& vertexProcessor, Light& light);

    void drawVertex(Rasterizer &rasterizer, VertexProcessor &vertexProcessor, Light& light);

private:
    void calculateNormals();

protected:
    std::vector<Vertex> mVertices;
    std::vector<int3> mIndices;
    Vertex mCenter;
};

