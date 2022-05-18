#include "point_light.hpp"
#include <algorithm>

float3 PointLight::calculate(const Fragment &fragment, VertexProcessor &vertexProcessor,std::shared_ptr<BMP> texture) const {
    auto L = mPosition - fragment.position;
    L.normalize();
    return doCalculate(L, fragment, vertexProcessor, texture);
}

PointLight::PointLight(const float3 &position, const float3 &ambient, const float3 &diffuse,
                                   const float3 &specular, float shininess) : Light(position, ambient, diffuse,
                                                                                    specular, shininess) {

}