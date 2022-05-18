#include "directional_light.hpp"
#include <algorithm>

float3 DirectionalLight::calculate(const Fragment &fragment, VertexProcessor &vertexProcessor, std::shared_ptr<BMP> texture) const {
    return doCalculate(mPosition, fragment, vertexProcessor, texture);
}

DirectionalLight::DirectionalLight(const float3 &position, const float3 &ambient, const float3 &diffuse,
                                   const float3 &specular, float shininess) : Light(position, ambient, diffuse,
                                                                                    specular, shininess) {

}
