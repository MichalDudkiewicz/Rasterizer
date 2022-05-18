cmake_minimum_required(VERSION 3.17)
project(untitled)

set(CMAKE_CXX_STANDARD 17)

add_executable(untitled main.cpp rasterizer.hpp rasterizer.cpp vector.cpp vertex_processor.cpp mesh.cpp vertex.hpp
        simple_triangle.cpp
        cone.cpp
        sphere.cpp
        light.cpp
        directional_light.cpp
        point_light.cpp
        )
