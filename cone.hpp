#pragma once

#include "mesh.hpp"

class Cone : public Mesh {
public:
    Cone(float r, float h, const Vertex &center, int v);
};

