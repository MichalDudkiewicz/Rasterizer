#pragma once
#include "vector.hpp"

struct Vertex
{
    float3 position;
    float3 normal;
    float3 textureCoords;
};

using Fragment = Vertex;
