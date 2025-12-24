//
// Created by Ingun Jon on 12/24/25.
//

#include "Map2DStaging.h"

#include <iostream>

#include "Image.h"
#include "my_io.h"

static nvrhi::Format format(const Image& img) {
    if (img.channel == 4) return nvrhi::Format::RGBA8_UNORM;
    throw std::runtime_error("invalid image channel");
}

AppPtr Map2DStaging::run() {
    auto image = Image::load("/Users/ingun/CLionProjects/nvrhi-unit-test/uv_grid_opengl_small_remainder.png");

    nvrhi::TextureDesc stagingTextureDesc{};
    stagingTextureDesc.width = image.width;
    stagingTextureDesc.height = image.height;
    stagingTextureDesc.format = format(image);
    auto stagingTexture = context.nvrhiDevice->
        createStagingTexture(stagingTextureDesc, nvrhi::CpuAccessMode::Write);

    std::cout << "Input Slice" << std::endl;
    auto sr = my_io::read_rect(0, 0, image.width, image.height);

    nvrhi::TextureSlice slice{};
    slice = slice.resolve(stagingTextureDesc);
    slice.x = sr.x;
    slice.y = sr.y;
    slice.width = sr.width;
    slice.height = sr.height;
    size_t pitch;
    const uint32_t actualRowSize = image.data.size() / image.height;
    auto mapPtr = static_cast<uint8_t*>(context.nvrhiDevice->mapStagingTexture(stagingTexture,
                                                                               slice,
                                                                               nvrhi::CpuAccessMode::Write,
                                                                               &pitch));
    std::cout << "Pitch: " << pitch << std::endl;
    for (int i = 0; i < image.height; ++i) {
        memcpy(mapPtr + (i * pitch),
               image.data.data() + (i * actualRowSize),
               actualRowSize);
    }

    context.nvrhiDevice->unmapStagingTexture(stagingTexture);

    auto commandList = context.nvrhiDevice->createCommandList();

    return std::make_unique<Map2DStaging>(*this);
}