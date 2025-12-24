//
// Created by Ingun Jon on 12/24/25.
//

#include "Map2DStaging.h"

#include <iostream>

#include "Image.h"
#include "my_math.h"
#include "my_io.h"
#include <algorithm>

static nvrhi::Format format(const Image& img) {
    if (img.channel == 4) return nvrhi::Format::RGBA8_UNORM;
    throw std::runtime_error("invalid image channel");
}

static nvrhi::TextureHandle create2DTexture(const uint32_t width,
                                            const uint32_t height,
                                            const nvrhi::Format format,
                                            const nvrhi::DeviceHandle& device) {
    nvrhi::TextureDesc dstTextureDesc{};
    dstTextureDesc.width = width;
    dstTextureDesc.height = height;
    dstTextureDesc.dimension = nvrhi::TextureDimension::Texture2D;
    dstTextureDesc.format = format;
    return device->createTexture(dstTextureDesc);
}

struct CommandSimpleCopy : public App {
    nvrhi::CommandListHandle commandList;
    nvrhi::StagingTextureHandle stagingTexture;

    CommandSimpleCopy() = delete;

    CommandSimpleCopy(const Context& webGPU,
                      nvrhi::CommandListHandle commandList,
                      nvrhi::StagingTextureHandle stagingTexture)
        : App(webGPU, "Execute command to copy staged buffer to texture"),
          commandList(std::move(commandList)),
          stagingTexture(std::move(stagingTexture)) {
    }

    AppPtr run() override {
        const uint32_t width = stagingTexture->getDesc().width;
        const uint32_t height = stagingTexture->getDesc().height;
        auto dstTexture = create2DTexture(width,
                                          height,
                                          stagingTexture->getDesc().format,
                                          context.nvrhiDevice);
        nvrhi::TextureSlice srcTextureSlice = nvrhi::TextureSlice().resolve(stagingTexture->getDesc());
        nvrhi::TextureSlice destSlice = nvrhi::TextureSlice().resolve(stagingTexture->getDesc());

        commandList->open();
        commandList->copyTexture(dstTexture, destSlice, stagingTexture, srcTextureSlice);
        commandList->close();
        context.nvrhiDevice->executeCommandList(commandList);

        return std::make_unique<CommandSimpleCopy>(*this);
    }
};

static nvrhi::StagingTextureHandle createStaging(uint32_t width,
                                                 uint32_t height,
                                                 const nvrhi::Format format,
                                                 const nvrhi::DeviceHandle& device) {
    nvrhi::TextureDesc stagingTextureDesc{};
    stagingTextureDesc.width = width;
    stagingTextureDesc.height = height;
    stagingTextureDesc.format = format;
    return device->createStagingTexture(stagingTextureDesc, nvrhi::CpuAccessMode::Write);
}

AppPtr Map2DStaging::run() {
    auto image = Image::load("/Users/ingun/CLionProjects/nvrhi-unit-test/uv_grid_opengl_small_remainder.png");

    std::cout << "Input Slice" << std::endl;
    auto sr = my_io::read_rect(0, 0, image.width, image.height);

    auto staging = createStaging(sr.width, sr.height, format(image), context.nvrhiDevice);

    nvrhi::TextureSlice slice{};
    slice = slice.resolve(staging->getDesc());
    size_t pitch;
    auto mapPtr = static_cast<uint8_t*>(context.nvrhiDevice->mapStagingTexture(staging,
                                                                               slice,
                                                                               nvrhi::CpuAccessMode::Write,
                                                                               &pitch));

    const uint32_t imageRowPitch = image.data.size() / image.height;
    const uint32_t pixelSize = imageRowPitch / image.width;

    std::cout << "Pitch: " << pitch << std::endl;
    for (uint32_t i = 0; i < sr.height; ++i) {
        memcpy(mapPtr + (i * staging->getDesc().width * pixelSize),
               image.data.data() + (i * imageRowPitch) + (sr.x * pixelSize),
               sr.width * pixelSize);
    }

    context.nvrhiDevice->unmapStagingTexture(staging);

    auto commandList = context.nvrhiDevice->createCommandList();

    return std::make_unique<CommandSimpleCopy>(context, commandList, staging);
}