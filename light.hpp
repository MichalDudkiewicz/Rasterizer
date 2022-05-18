#pragma once

#include <memory>
#include "vector.hpp"
#include "vertex_processor.hpp"
#include "vertex.hpp"

class BMP;

class Light {
public:
    Light(const float3& position, const float3& ambient, const float3& diffuse, const float3& specular, float shininess);

public:
    virtual float3 calculate(const Fragment &fragment, VertexProcessor& vertexProcessor, std::shared_ptr<BMP> texture = nullptr) const = 0;

protected:
    virtual float3 doCalculate(const float3& lightDir, const Fragment &fragment, VertexProcessor& vertexProcessor, std::shared_ptr<BMP> texture = nullptr) const;

protected:
    float3 mPosition;
    float3 mAmbient;
    float3 mDiffuse;
    float3 mSpecular;
    float mShininess;
};

