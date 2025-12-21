//
// Created by Ingun Jon on 12/21/25.
//

#include "Image.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Image Image::load(const std::string& path) {
    int imgW, imgH, imgC;
    const char* img_path = "/Users/ingun/CLionProjects/nvrhi-unit-test/uv_grid_opengl.png";
    const unsigned char* img = stbi_load(img_path, &imgW, &imgH, &imgC, 4);
    if (imgC != 4) throw std::runtime_error("invalid image channel");
    std::vector<uint8_t> data(img, img + imgW * imgH * 4);
    return {imgW, imgH, imgC, std::move(data)};
}