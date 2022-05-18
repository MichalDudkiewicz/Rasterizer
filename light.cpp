#include "light.hpp"
#include <algorithm>
#include "BMP.h"

Light::Light(const float3 &position, const float3 &ambient, const float3 &diffuse, const float3 &specular,
             float shininess)
             : mPosition(position), mAmbient(ambient), mDiffuse(diffuse), mSpecular(specular), mShininess(shininess)
             {

}

float3 Light::doCalculate(const float3 &lightDir, const Fragment &fragment, VertexProcessor &vertexProcessor, std::shared_ptr<BMP> texture) const {
    auto N = fragment.normal;
    N.normalize();
    auto V = fragment.position;
    V.normalize();
    V.negate();

    Vector L = lightDir;
    L.normalize();

    float shade = std::clamp(N.dotProduct(L), 0.0f, 1.0f);
    const float3 diffuse{shade * mDiffuse.r(), shade * mDiffuse.g(), shade * mDiffuse.b()};

    float shine = 0.0f;
    if (L.dotProduct(N) >= 0.0f)
    {
        auto R = (N * N.dotProduct(L) * 2.0f) - L;
        R.normalize();
        shine = std::clamp(R.dotProduct(V), 0.0f, 1.0f);
        shine = powf(shine, mShininess);
    }
    const float3 specular{shine * mSpecular.r(), shine * mSpecular.g(), shine * mSpecular.b()};

    auto sum = mAmbient + specular + diffuse;

    if (texture)
    {
        const auto t = texture->get_pixel(std::clamp(fragment.textureCoords.x() * texture->bmp_info_header.width, 0.0f, (float)texture->bmp_info_header.width-1), std::clamp(fragment.textureCoords.y() * texture->bmp_info_header.height, 0.0f, (float)texture->bmp_info_header.height-1));
        sum += t;
    }

    sum[0] = std::clamp(sum[0], 0.0f, 1.0f);
    sum[1] = std::clamp(sum[1], 0.0f, 1.0f);
    sum[2] = std::clamp(sum[2], 0.0f, 1.0f);
    return sum;
}


