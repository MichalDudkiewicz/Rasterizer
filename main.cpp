#include <iostream>
#include "BMP.h"
#include "rasterizer.hpp"
#include "vector.hpp"
#include "vertex_processor.hpp"
#include "mesh.hpp"
#include "simple_triangle.hpp"
#include "cone.hpp"
#include "sphere.hpp"
#include "directional_light.hpp"
#include "point_light.hpp"

int main() {
    VertexProcessor vertexProcessor;
    vertexProcessor.setPerspective(120, 1, 0.5, 100);
	BMP bmp2(400, 400, vertexProcessor);
    Rasterizer rasterizer(bmp2);
    Vertex vertexCenter;
    vertexCenter.position.z() = -2.0f;
    const float3 eye{8.0f, 0.0f, -5.0f};
    float3 up{0, 1, 0};

    // or direction
    float3 position = float3{0, 1, 0};
    float3 ambient= float3{0.1, 0.1, 0.1};
    float3 diffuse= float3{0.4, 0.4, 0.4};
    float3 specular= float3{0.5, 0.5, 0.5};
    float shininess = 12.f;
//    DirectionalLight light(position, ambient, diffuse, specular, shininess);
    PointLight noLight(position, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, 0.0f);
    PointLight light(position, ambient, diffuse, specular, shininess);

    bmp2.loadTexture("moon.bmp");
    Vertex sphereCenter;
    sphereCenter.position.z() = -1.5f;
    Sphere sphere(10, 10, sphereCenter, .5f);
    sphere.draw(rasterizer, vertexProcessor, light);

    Vertex sphereCenter2;
    sphereCenter2.position = float3{-1.0f, 0.0f, -1.0f};
    Sphere sphere2(10, 10, sphereCenter2, .5f);

    bmp2.loadTexture("earth.bmp");

    sphere2.draw(rasterizer, vertexProcessor, light);

    Vertex sphereCenter3;
    sphereCenter3.position = float3{1.0f, 0.0f, -1.0f};
    Sphere sphere3(10, 10, sphereCenter3, .5f);

    sphere3.draw(rasterizer, vertexProcessor, noLight);
	bmp2.write("img_test.bmp");
    return 0;
}
