#include "cone.hpp"

Cone::Cone(float r, float h, const Vertex &center, int v) : Mesh(v+2, 2 * v, center){
    mVertices[0]=center;
    Vertex peak = center;
    peak.position.z() -= h;
    mVertices[1] = peak;
    float step = (2 * M_PIf32) / v;
    constexpr int firstBaseIndice = 2;
    int k = firstBaseIndice;
    for (float t = 0 ; t < 2 * M_PIf32 ; t += step )
    {
        Vertex vertex;
        vertex.position = float3{r * cosf(t) + center.position.x(), r*sinf(t) + center.position.y() ,center.position.z()};
        mVertices[k] = vertex;
        k++;
    }

    k = 0;
    for (int i = firstBaseIndice; i < mVertices.size(); i++)
    {
        int j = i+1;
        if (j >= mVertices.size())
        {
            j = firstBaseIndice;
        }
        int3 indexes2{j, firstBaseIndice-2, i};
        mIndices[k] = indexes2;
        int3 indexesPeak{i, firstBaseIndice-1, j};
        mIndices[k+1] = indexesPeak;
        k+=2;
    }
}
