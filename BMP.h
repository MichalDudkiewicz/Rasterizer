#pragma once
#include <fstream>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <cmath>
#include <memory>
#include "vector.hpp"
#include "light.hpp"
#include "vertex_processor.hpp"

#pragma pack(push, 1)
struct BMPFileHeader {
    uint16_t file_type{ 0x4D42 };          // File type always BM which is 0x4D42 (stored as hex uint16_t in little endian)
    uint32_t file_size{ 0 };               // Size of the file (in bytes)
    uint16_t reserved1{ 0 };               // Reserved, always 0
    uint16_t reserved2{ 0 };               // Reserved, always 0
    uint32_t offset_data{ 0 };             // Start position of pixel data (bytes from the beginning of the file)
};

struct BMPInfoHeader {
    uint32_t size{ 0 };                      // Size of this header (in bytes)
    int32_t width{ 0 };                      // width of bitmap in pixels
    int32_t height{ 0 };                     // width of bitmap in pixels
                                             //       (if positive, bottom-up, with origin in lower left corner)
                                             //       (if negative, top-down, with origin in upper left corner)
    uint16_t planes{ 1 };                    // No. of planes for the target device, this is always 1
    uint16_t bit_count{ 0 };                 // No. of bits per pixel
    uint32_t compression{ 0 };               // 0 or 3 - uncompressed. THIS PROGRAM CONSIDERS ONLY UNCOMPRESSED BMP images
    uint32_t size_image{ 0 };                // 0 - for uncompressed images
    int32_t x_pixels_per_meter{ 0 };
    int32_t y_pixels_per_meter{ 0 };
    uint32_t colors_used{ 0 };               // No. color indexes in the color table. Use 0 for the max number of colors allowed by bit_count
    uint32_t colors_important{ 0 };          // No. of colors used for displaying the bitmap. If 0 all colors are required
};

struct BMPColorHeader {
    uint32_t red_mask{ 0x00ff0000 };         // Bit mask for the red channel
    uint32_t green_mask{ 0x0000ff00 };       // Bit mask for the green channel
    uint32_t blue_mask{ 0x000000ff };        // Bit mask for the blue channel
    uint32_t alpha_mask{ 0xff000000 };       // Bit mask for the alpha channel
    uint32_t color_space_type{ 0x73524742 }; // Default "sRGB" (0x73524742)
    uint32_t unused[16]{ 0 };                // Unused data for sRGB color space
};
#pragma pack(pop)

struct BMP {
    BMPFileHeader file_header;
    BMPInfoHeader bmp_info_header;
    BMPColorHeader bmp_color_header;
    VertexProcessor& mVertexProcessor;
    std::vector<uint8_t> data;
    std::vector<std::vector<float>> depth_buffer = {};
    std::shared_ptr<BMP> mTexture;

    BMP(const char *fname, VertexProcessor& vertexProcessor) : mVertexProcessor(vertexProcessor) {
        read(fname);
    }



    void read(const char *fname) {
        std::ifstream inp{ fname, std::ios_base::binary };
        if (inp) {
            inp.read((char*)&file_header, sizeof(file_header));
            if(file_header.file_type != 0x4D42) {
                throw std::runtime_error("Error! Unrecognized file format.");
            }
            inp.read((char*)&bmp_info_header, sizeof(bmp_info_header));

            // The BMPColorHeader is used only for transparent images
            if(bmp_info_header.bit_count == 32) {
                // Check if the file has bit mask color information
                if(bmp_info_header.size >= (sizeof(BMPInfoHeader) + sizeof(BMPColorHeader))) {
                    inp.read((char*)&bmp_color_header, sizeof(bmp_color_header));
                    // Check if the pixel data is stored as BGRA and if the color space type is sRGB
                    check_color_header(bmp_color_header);
                } else {
                    std::cerr << "Error! The file \"" << fname << "\" does not seem to contain bit mask information\n";
                    throw std::runtime_error("Error! Unrecognized file format.");
                }
            }

            // Jump to the pixel data location
            inp.seekg(file_header.offset_data, inp.beg);

            // Adjust the header fields for output.
            // Some editors will put extra info in the image file, we only save the headers and the data.
            if(bmp_info_header.bit_count == 32) {
                bmp_info_header.size = sizeof(BMPInfoHeader) + sizeof(BMPColorHeader);
                file_header.offset_data = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader) + sizeof(BMPColorHeader);
            } else {
                bmp_info_header.size = sizeof(BMPInfoHeader);
                file_header.offset_data = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader);
            }
            file_header.file_size = file_header.offset_data;

            if (bmp_info_header.height < 0) {
                throw std::runtime_error("The program can treat only BMP images with the origin in the bottom left corner!");
            }

            data.resize(bmp_info_header.width * bmp_info_header.height * bmp_info_header.bit_count / 8);

            // Here we check if we need to take into account row padding
            if (bmp_info_header.width % 4 == 0) {
                inp.read((char*)data.data(), data.size());
                file_header.file_size += static_cast<uint32_t>(data.size());
            }
            else {
                row_stride = bmp_info_header.width * bmp_info_header.bit_count / 8;
                uint32_t new_stride = make_stride_aligned(4);
                std::vector<uint8_t> padding_row(new_stride - row_stride);

                for (int y = 0; y < bmp_info_header.height; ++y) {
                    inp.read((char*)(data.data() + row_stride * y), row_stride);
                    inp.read((char*)padding_row.data(), padding_row.size());
                }
                file_header.file_size += static_cast<uint32_t>(data.size()) + bmp_info_header.height * static_cast<uint32_t>(padding_row.size());
            }
        }
        else {
            throw std::runtime_error("Unable to open the input image file.");
        }
    }

    void fill_triangle_vertex(int x1, int y1, float z1, const float3& vertexColor1, int x2, int y2, float z2, const float3& vertexColor2, int x3, int y3, float z3, const float3& vertexColor3) {

        int channels = bmp_info_header.bit_count / 8;

        const int minx = std::max(std::min(std::min(x1, x2), x3), 0);
        const int maxx = std::min(std::max(std::max(x1, x2), x3), bmp_info_header.width - 1);
        const int miny = std::max(std::min(std::min(y1, y2), y3), 0);
        const int maxy = std::min(std::max(std::max(y1, y2), y3), bmp_info_header.height - 1);

        const int dx12 = x1 - x2;
        const int dx23 = x2 - x3;
        const int dx31 = x3 - x1;
        const int dy12 = y1 - y2;
        const int dy23 = y2 - y3;
        const int dy31 = y3 - y1;

        const bool tl1 = dy12 < 0 || (dy12 == 0 && dx12 > 0);
        const bool tl2 = dy23 < 0 || (dy23 == 0 && dx23 > 0);
        const bool tl3 = dy31 < 0 || (dy31 == 0 && dx31 > 0);

        for (int y = miny; y <= maxy; ++y) {
            for (int x = minx; x <= maxx; ++x) {

                const float lambda1 = (float)(dy23*(x - x3)+(x3-x2)*(y-y3))/(float)(dy23*(x1-x3)+(x3-x2)*(y1-y3));
                const float lambda2 = (float)(dy31*(x-x3)+(x1-x3)*(y-y3))/(float)(dy31*dx23+(x1-x3)*dy23);
                const float lambda3 = 1 - lambda1 - lambda2;

                const int cond1 = dx12 * (y - y1) - (dy12) * (x - x1);
                const int cond2 = dx23 * (y - y2) - (dy23) * (x - x2);
                const int cond3 = dx31 * (y - y3) - (dy31) * (x - x3);
                const float depth = lambda1 * z1 + lambda2 * z2 + lambda3 * z3;

                if (
                        (cond1 >= 0 && tl1 || cond1 > 0 && !tl1) &&
                        (cond2 >= 0 && tl2 || cond2 > 0 && !tl2) &&
                        (cond3 >= 0 && tl3 || cond3 > 0 && !tl3) &&
                        depth < depth_buffer[x][y]
                        )
                {
                    depth_buffer[x][y] = depth;
                    data[channels * (y * bmp_info_header.width + x) + 0] = (int)((lambda1 * vertexColor1.b() + lambda2 * vertexColor2.b() + lambda3 * vertexColor3.b()) * 255);
                    data[channels * (y * bmp_info_header.width + x) + 1] = (int)((lambda1 * vertexColor1.g() + lambda2 * vertexColor2.g() + lambda3 * vertexColor3.g()) * 255);
                    data[channels * (y * bmp_info_header.width + x) + 2] = (int)((lambda1 * vertexColor1.r() + lambda2 * vertexColor2.r() + lambda3 * vertexColor3.r()) * 255);
                    if (channels == 4) {
                        data[channels * (y * bmp_info_header.width + x) + 3] = 255;
                    }
                }
            }
        }
    }

    BMP(int32_t width, int32_t height, VertexProcessor& vertexProcessor, bool has_alpha = true) : mVertexProcessor(vertexProcessor) {
        if (width <= 0 || height <= 0) {
            throw std::runtime_error("The image width and height must be positive numbers.");
        }

        bmp_info_header.width = width;
        bmp_info_header.height = height;
        if (has_alpha) {
            bmp_info_header.size = sizeof(BMPInfoHeader) + sizeof(BMPColorHeader);
            file_header.offset_data = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader) + sizeof(BMPColorHeader);

            bmp_info_header.bit_count = 32;
            bmp_info_header.compression = 3;
            row_stride = width * 4;
            data.resize(row_stride * height);
            file_header.file_size = file_header.offset_data + data.size();
        }
        else {
            bmp_info_header.size = sizeof(BMPInfoHeader);
            file_header.offset_data = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader);

            bmp_info_header.bit_count = 24;
            bmp_info_header.compression = 0;
            row_stride = width * 3;
            data.resize(row_stride * height);

            uint32_t new_stride = make_stride_aligned(4);
            file_header.file_size = file_header.offset_data + static_cast<uint32_t>(data.size()) + bmp_info_header.height * (new_stride - row_stride);
        }

        fill_region(0, 0, bmp_info_header.width, bmp_info_header.height, 0, 0, 0, 255);
        depth_buffer = std::vector<std::vector<float>>(bmp_info_header.width, std::vector<float>(bmp_info_header.height, 1.0f));

    }

    void write(const char *fname) {
        std::ofstream of{ fname, std::ios_base::binary };
        if (of) {
            if (bmp_info_header.bit_count == 32) {
                write_headers_and_data(of);
            }
            else if (bmp_info_header.bit_count == 24) {
                if (bmp_info_header.width % 4 == 0) {
                    write_headers_and_data(of);
                }
                else {
                    uint32_t new_stride = make_stride_aligned(4);
                    std::vector<uint8_t> padding_row(new_stride - row_stride);

                    write_headers(of);

                    for (int y = 0; y < bmp_info_header.height; ++y) {
                        of.write((const char*)(data.data() + row_stride * y), row_stride);
                        of.write((const char*)padding_row.data(), padding_row.size());
                    }
                }
            }
            else {
                throw std::runtime_error("The program can treat only 24 or 32 bits per pixel BMP files");
            }
        }
        else {
            throw std::runtime_error("Unable to open the output image file.");
        }
    }

    void loadTexture(const char *fname)
    {
        BMP texture(fname, mVertexProcessor);
        mTexture = std::make_shared<BMP>(texture);
    }

    void fill_triangle(int x1, int y1, float z1, const float3& normal1, int x2, int y2, float z2, const float3& normal2, int x3, int y3, float z3, const float3& normal3, const Light& light, const std::vector<float3>& positions, const Vertex& f1, const Vertex& f2, const Vertex& f3) {

        int channels = bmp_info_header.bit_count / 8;

        const int minx = std::max(std::min(std::min(x1, x2), x3), 0);
        const int maxx = std::min(std::max(std::max(x1, x2), x3), bmp_info_header.width - 1);
        const int miny = std::max(std::min(std::min(y1, y2), y3), 0);
        const int maxy = std::min(std::max(std::max(y1, y2), y3), bmp_info_header.height - 1);

        const int dx12 = x1 - x2;
        const int dx23 = x2 - x3;
        const int dx31 = x3 - x1;
        const int dy12 = y1 - y2;
        const int dy23 = y2 - y3;
        const int dy31 = y3 - y1;

        const bool tl1 = dy12 < 0 || (dy12 == 0 && dx12 > 0);
        const bool tl2 = dy23 < 0 || (dy23 == 0 && dx23 > 0);
        const bool tl3 = dy31 < 0 || (dy31 == 0 && dx31 > 0);

        for (int y = miny; y <= maxy; ++y) {
            for (int x = minx; x <= maxx; ++x) {

                const float lambda1 = (float)(dy23*(x - x3)+(x3-x2)*(y-y3))/(float)(dy23*(x1-x3)+(x3-x2)*(y1-y3));
                const float lambda2 = (float)(dy31*(x-x3)+(x1-x3)*(y-y3))/(float)(dy31*dx23+(x1-x3)*dy23);
                const float lambda3 = 1 - lambda1 - lambda2;

                const int cond1 = dx12 * (y - y1) - (dy12) * (x - x1);
                const int cond2 = dx23 * (y - y2) - (dy23) * (x - x2);
                const int cond3 = dx31 * (y - y3) - (dy31) * (x - x3);
                const float depth = lambda1 * z1 + lambda2 * z2 + lambda3 * z3;

                if (
                    (cond1 >= 0 && tl1 || cond1 > 0 && !tl1) &&
                    (cond2 >= 0 && tl2 || cond2 > 0 && !tl2) &&
                    (cond3 >= 0 && tl3 || cond3 > 0 && !tl3) &&
                    depth < depth_buffer[x][y]
                    )
                {
                    depth_buffer[x][y] = depth;
                    auto normal = normal1 * lambda1 + normal2 * lambda2 + normal3 * lambda3;
                    normal.normalize();
                    Fragment fragment;
                    fragment.normal = normal;
                    auto position = positions[0] * lambda1 + positions[1] * lambda2 + positions[2] * lambda3;
                    fragment.position = position;
                    fragment.textureCoords = f1.textureCoords * lambda1 + f2.textureCoords * lambda2 + f3.textureCoords * lambda3;
                    const auto vertexColors = light.calculate(fragment, mVertexProcessor, mTexture);
                    data[channels * (y * bmp_info_header.width + x) + 0] = (int)((vertexColors.b()) * 255);
                    data[channels * (y * bmp_info_header.width + x) + 1] = (int)((vertexColors.g()) * 255);
                    data[channels * (y * bmp_info_header.width + x) + 2] = (int)((vertexColors.r()) * 255);
                    if (channels == 4) {
                        data[channels * (y * bmp_info_header.width + x) + 3] = 255;
                    }
                }
            }
        }
    }

    void fill_region(uint32_t x0, uint32_t y0, uint32_t w, uint32_t h, uint8_t B, uint8_t G, uint8_t R, uint8_t A) {
        if (x0 + w > (uint32_t)bmp_info_header.width || y0 + h > (uint32_t)bmp_info_header.height) {
            throw std::runtime_error("The region does not fit in the image!");
        }

        uint32_t channels = bmp_info_header.bit_count / 8;
        for (uint32_t y = y0; y < y0 + h; ++y) {
            for (uint32_t x = x0; x < x0 + w; ++x) {
                data[channels * (y * bmp_info_header.width + x) + 0] = B;
                data[channels * (y * bmp_info_header.width + x) + 1] = G;
                data[channels * (y * bmp_info_header.width + x) + 2] = R;
                if (channels == 4) {
                    data[channels * (y * bmp_info_header.width + x) + 3] = A;
                }
            }
        }
    }

    void fill_pixel(uint32_t x0, uint32_t y0, uint8_t B, uint8_t G, uint8_t R, uint8_t A) {
        if (x0 > (uint32_t)bmp_info_header.width || y0 > (uint32_t)bmp_info_header.height) {
            throw std::runtime_error("The pixel does not fit in the image!");
        }

        uint32_t channels = bmp_info_header.bit_count / 8;
        data[channels * (y0 * bmp_info_header.width + x0) + 0] = B;
        data[channels * (y0 * bmp_info_header.width + x0) + 1] = G;
        data[channels * (y0 * bmp_info_header.width + x0) + 2] = R;
        if (channels == 4) {
            data[channels * (y0 * bmp_info_header.width + x0) + 3] = A;
        }
    }

    void set_pixel(uint32_t x0, uint32_t y0, uint8_t B, uint8_t G, uint8_t R, uint8_t A) {
        if (x0 >= (uint32_t)bmp_info_header.width || y0 >= (uint32_t)bmp_info_header.height || x0 < 0 || y0 < 0) {
            throw std::runtime_error("The point is outside the image boundaries!");
        }

        uint32_t channels = bmp_info_header.bit_count / 8;
        data[channels * (y0 * bmp_info_header.width + x0) + 0] = B;
        data[channels * (y0 * bmp_info_header.width + x0) + 1] = G;
        data[channels * (y0 * bmp_info_header.width + x0) + 2] = R;
        if (channels == 4) {
            data[channels * (y0 * bmp_info_header.width + x0) + 3] = A;
        }
    }

    float3 get_pixel(uint32_t x0, uint32_t y0) {
        if (x0 >= (uint32_t)bmp_info_header.width || y0 >= (uint32_t)bmp_info_header.height || x0 < 0 || y0 < 0) {
            throw std::runtime_error("The point is outside the image boundaries!");
        }

        uint32_t channels = bmp_info_header.bit_count / 8;
        uint8_t B = data[channels * (y0 * bmp_info_header.width + x0) + 0];
        uint8_t G = data[channels * (y0 * bmp_info_header.width + x0) + 1];
        uint8_t R = data[channels * (y0 * bmp_info_header.width + x0) + 2];

        float3 color = float3{ (float)R / 255.0f, (float)G / 255.0f, (float)B / 255.0f};

        return color;
    }

    void draw_rectangle(uint32_t x0, uint32_t y0, uint32_t w, uint32_t h,
                        uint8_t B, uint8_t G, uint8_t R, uint8_t A, uint8_t line_w) {
        if (x0 + w > (uint32_t)bmp_info_header.width || y0 + h > (uint32_t)bmp_info_header.height) {
            throw std::runtime_error("The rectangle does not fit in the image!");
        }

        fill_region(x0, y0, w, line_w, B, G, R, A);                                             // top line
        fill_region(x0, (y0 + h - line_w), w, line_w, B, G, R, A);                              // bottom line
        fill_region((x0 + w - line_w), (y0 + line_w), line_w, (h - (2 * line_w)), B, G, R, A);  // right line
        fill_region(x0, (y0 + line_w), line_w, (h - (2 * line_w)), B, G, R, A);                 // left line
    }

private:
    uint32_t row_stride{ 0 };

    void write_headers(std::ofstream &of) {
        of.write((const char*)&file_header, sizeof(file_header));
        of.write((const char*)&bmp_info_header, sizeof(bmp_info_header));
        if(bmp_info_header.bit_count == 32) {
            of.write((const char*)&bmp_color_header, sizeof(bmp_color_header));
        }
    }

    void write_headers_and_data(std::ofstream &of) {
        write_headers(of);
        of.write((const char*)data.data(), data.size());
    }

    // Add 1 to the row_stride until it is divisible with align_stride
    uint32_t make_stride_aligned(uint32_t align_stride) {
        uint32_t new_stride = row_stride;
        while (new_stride % align_stride != 0) {
            new_stride++;
        }
        return new_stride;
    }

    // Check if the pixel data is stored as BGRA and if the color space type is sRGB
    void check_color_header(BMPColorHeader &bmp_color_header) {
        BMPColorHeader expected_color_header;
        if(expected_color_header.red_mask != bmp_color_header.red_mask ||
            expected_color_header.blue_mask != bmp_color_header.blue_mask ||
            expected_color_header.green_mask != bmp_color_header.green_mask ||
            expected_color_header.alpha_mask != bmp_color_header.alpha_mask) {
            throw std::runtime_error("Unexpected color mask format! The program expects the pixel data to be in the BGRA format");
        }
        if(expected_color_header.color_space_type != bmp_color_header.color_space_type) {
            throw std::runtime_error("Unexpected color space type! The program expects sRGB values");
        }
    }
};
