#pragma once

#include "light.hpp"

class DirectionalLight : public Light{
public:
    DirectionalLight(const float3& position, const float3& ambient, const float3& diffuse, const float3& specular, float shininess);

    float3 calculate(const Fragment &fragment, VertexProcessor &vertexProcessor, std::shared_ptr<BMP> texture = nullptr) const override;
};

