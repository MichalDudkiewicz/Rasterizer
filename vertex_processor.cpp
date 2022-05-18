#include "vertex_processor.hpp"
#include "vector.hpp"
#include <cmath>

void VertexProcessor::setPerspective(float fovy, float aspect, float near, float far) {
    fovy *= M_PI / 360; // FOVy/2
    float f = std::cos( fovy ) / std::sin(fovy) ;
    mView2Proj[ 0 ] = float4 {f / aspect , 0 , 0 , 0 } ;
    mView2Proj[ 1 ] = float4 {0 , f , 0 , 0 } ;
    mView2Proj[ 2 ] = float4 { 0 , 0 , ( far+near ) / ( near-far ) , -1} ;
    mView2Proj[ 3 ] = float4 { 0 , 0 , 2*far* near / ( near-far ) , 0 };
}

float3 VertexProcessor::convertToCanonical(const float3 &worldCoords) const {
    float4 coords({worldCoords.x(), worldCoords.y(), worldCoords.z(), 1.0f});
    coords *= mObj2World;
    coords *= mWorld2View;
    coords *= mView2Proj;
    coords /= coords.w();
    return {coords.x(), coords.y(), coords.z()};
}

void VertexProcessor::setLookAt(float3 eye, float3 center, float3 up) {
    float3 f = center - eye;
    f.normalize();
    up.normalize();
    float3 s = crossProduct(f ,up);
    float3 u = crossProduct(s,f);
    mWorld2View [ 0 ] = float4 { s [ 0 ] , u [ 0 ] , -f [ 0 ] , 0 } ;
    mWorld2View [ 1 ] = float4 { s [ 1 ] , u [ 1 ] , -f [ 1 ] , 0 } ;
    mWorld2View [ 2 ] = float4 { s [ 2 ] , u [ 2 ] , -f [ 2 ] , 0 } ;
    mWorld2View [ 3 ] = float4 { 0 , 0 , 0 , 1 } ;
    float4x4 m; // i d e n t i t y
    m[0][0] = 1.0f;
    m[1][1] = 1.0f;
    m[2][2] = 1.0f;
    eye.negate();
    m [ 3 ] = float4 {eye.x(), eye.y(), eye.z() , 1 } ;
    mWorld2View *= m;
}

void VertexProcessor::multByTranslation(float3 v) {
    float4x4 m;
    m[0] = float4 {1 , 0 , 0 , 0 };
    m[1] = float4 { 0 , 1 , 0 , 0 };
    m[2] = float4 { 0 , 0 , 1 , 0 };
    m[3] = float4{v.x() , v.y() , v.z() , 1};
    mObj2World = m * mObj2World;
}

void VertexProcessor::multByScale( float3 v )
{
    float4x4 m;
    m[0] = float4 {v.x() , 0 , 0 , 0 };
    m[1] = float4 { 0 , v.y() , 0 , 0 };
    m[2] = float4 { 0 , 0 , v.z() , 0 };
    m[3] = float4{0, 0, 0 , 1};
    mObj2World = m * mObj2World;
}

void VertexProcessor::multByRotation(float a, float3 v) {
    float s=sinf( a * M_PIf32 / 180 );
    float c=cosf( a * M_PIf32 / 180 ) ;
    v.normalize();
    float4x4 m;
    m[0] = float4 {v.x()*v.x()* (1 - c )+c , v.y() * v.x() * (1 -c )+v.z() * s ,
    v.x() * v.z() * (1 - c ) - v.y() * s , 0 };

    m[1] = float4 { v.x() * v.y() * (1 - c ) - v.z() * s , v.y() * v.y() * (1 - c )+c ,
    v.y() * v.z() * (1 - c )+v.x() * s , 0 };

    m[2] = float4 { v.x() * v.z() * (1 - c )+v.y() * s , v.y() * v.z() * (1 - c ) - v.x() * s ,
    v.z() * v.z() * (1 - c )+c , 0 };

    m[3] = float4{0, 0, 0 , 1};
    mObj2World = m * mObj2World;
}


