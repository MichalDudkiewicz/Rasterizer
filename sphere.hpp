#pragma once

#include "mesh.hpp"

class Sphere : public Mesh {
public:
    Sphere(int horiz, int vert, const Vertex &center, float radius);
};

