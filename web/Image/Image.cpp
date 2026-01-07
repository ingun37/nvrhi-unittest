//
// Created by Ingun Jon on 12/21/25.
//

#include "Image.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <random>
#include <algorithm>

Image Image::load(const std::string& path) {
    int imgW, imgH, imgC;
    const char* img_path = path.c_str();
    const unsigned char* img = stbi_load(img_path, &imgW, &imgH, &imgC, 4);
    if (imgC != 4) throw std::runtime_error("invalid image channel");
    std::vector<uint8_t> data(img, img + imgW * imgH * 4);
    return {imgW, imgH, imgC, std::move(data)};
}

Image Image::create(int width, int height, int channel, ComponentType componentType) {
    size_t compSize = componentTypeSize(componentType);
    std::vector<uint8_t> data(width * height * channel * compSize);

    std::random_device rd;
    std::mt19937 gen(rd());

    if (componentType == ComponentType::Float) {
        std::uniform_real_distribution<float> dis(0.0f, 1.0f);
        float* floatData = reinterpret_cast<float*>(data.data());
        for (size_t i = 0; i < width * height * channel; ++i) {
            floatData[i] = dis(gen);
        }
    } else {
        std::uniform_int_distribution<uint16_t> dis(0, 255);
        for (size_t i = 0; i < data.size(); ++i) {
            data[i] = static_cast<uint8_t>(dis(gen));
        }
    }

    return Image(width, height, channel, std::move(data));
}