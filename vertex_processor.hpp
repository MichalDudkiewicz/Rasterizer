#pragma once

#include "vector.hpp"

class VertexProcessor {
public:
    void setPerspective(float fovy, float aspect, float near, float far);

    void setLookAt(float3 eye , float3 center , float3 up);

    void multByTranslation(float3 v);

    void multByScale( float3 v );

    void multByRotation( float a , float3 v );

    float3 convertToCanonical(const float3& worldCoords) const;

private:
    float4x4 mView2Proj;
    float4x4 mWorld2View{{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}};
    float4x4 mObj2World{{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}};
};

