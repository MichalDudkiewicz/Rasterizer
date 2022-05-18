#include "sphere.hpp"
#include <cmath>
#include "vector.hpp"

Sphere::Sphere(int horiz, int vert, const Vertex &center, float radius) : Mesh(vert * ( horiz + 2 ), 2 * vert * horiz, center) {
    for (int yy = 0; yy <= horiz + 1; ++yy) {
        float y = cosf(yy * M_PIf32 / (horiz + 1.0f));
        float r = sqrtf(1 - y * y);
        for (int rr = 0; rr < vert; ++rr) {
            float x = r * cosf(2 * M_PIf32 * rr / vert);
            float z = r * sinf(2 * M_PIf32 * rr / vert);
            mVertices[rr + yy * vert].position = (float3{x, y, z} * radius) + center.position;
        }
    }

    for (int yy = 0; yy < horiz; ++yy)
    {
        for (int rr = 0; rr < vert; ++rr) {
            mIndices[ rr + 2 * yy * vert ] = int3{
                    (rr + 1) % vert
                    + yy * vert,
                    rr + vert
                    + yy * vert,
                    (rr + 1) % vert + vert + yy * vert
            };
            mIndices[ rr + vert + 2 * yy * vert ] = int3{
                    rr + vert
                    + yy * vert,
                    rr + 2 * vert
                    + yy * vert,
                    (rr + 1) % vert + vert + yy * vert
            };
        }
    }
}
