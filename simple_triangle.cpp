#include "simple_triangle.hpp"

SimpleTriangle::SimpleTriangle() : Mesh(3, 1, {}){
    mVertices[ 0 ].position = float3 {-.5f , 0.f , 0.f };
    mVertices[ 1 ].position = float3 { 0.f , .5f , 0.f };
    mVertices[ 2 ].position = float3 { .5f , 0.f , 0.f };
    mIndices[ 0 ] = int3{0 , 1 , 2 };
}
