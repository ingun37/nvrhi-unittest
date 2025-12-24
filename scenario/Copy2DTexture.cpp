//
// Created by Ingun Jon on 12/24/25.
//

#include "Copy2DTexture.h"
#include <iostream>
#include <sstream>
#include <my_io.h>

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

static nvrhi::Format format(const Image& img) {
    if (img.channel == 4) return nvrhi::Format::RGBA8_UNORM;
    throw std::runtime_error("invalid image channel");
}
struct CommandExecution : public App {
    nvrhi::CommandListHandle commandList;
    nvrhi::StagingTextureHandle stagingTexture;

    CommandExecution() = delete;

    CommandExecution(const Context& webGPU,
                     nvrhi::CommandListHandle commandList,
                     nvrhi::StagingTextureHandle stagingTexture)
        : App(webGPU, "Execute command to copy staged buffer to texture"),
          commandList(std::move(commandList)),
          stagingTexture(std::move(stagingTexture)) {
    }

    AppPtr run() override;
};



AppPtr CommandExecution::run() {
    uint32_t original_width = stagingTexture->getDesc().width;
    uint32_t original_height = stagingTexture->getDesc().height;
    std::cout << "Input Source Slice" << std::endl;
    auto sr = my_io::read_rect(0, 0, original_width, original_height);
    std::cout << "Input Destination Slice" << std::endl;
    auto dr = my_io::read_rect(120, 300, original_width * 0.75f, original_height * 0.333f);

    nvrhi::TextureSlice destSlice{};
    destSlice.x = dr.x;
    destSlice.y = dr.y;
    destSlice.width = dr.width;
    destSlice.height = dr.height;
    destSlice.depth = 1;
    nvrhi::TextureSlice srcTextureSlice{};
    srcTextureSlice.x = sr.x;
    srcTextureSlice.y = sr.y;
    srcTextureSlice.width = sr.width;
    srcTextureSlice.height = sr.height;
    srcTextureSlice.depth = 1;

    auto dstTexture = create2DTexture(original_width * 2,
                                      original_height * 2,
                                      stagingTexture->getDesc().format,
                                      context.nvrhiDevice);
    commandList->open();
    commandList->copyTexture(dstTexture, destSlice, stagingTexture, srcTextureSlice);
    commandList->close();
    context.nvrhiDevice->executeCommandList(commandList);
    return std::make_unique<CommandExecution>(*this);
}

Copy2D::Copy2D(const Context& webGpu)
    : App(webGpu, "Load image from file") {
}

AppPtr Copy2D::run() {
    auto image = Image::load("/Users/ingun/CLionProjects/nvrhi-unit-test/uv_grid_opengl_small_remainder.png");

    nvrhi::TextureDesc stagingTextureDesc{};
    stagingTextureDesc.width = image.width;
    stagingTextureDesc.height = image.height;
    stagingTextureDesc.format = format(image);
    auto stagingTexture = context.nvrhiDevice->
        createStagingTexture(stagingTextureDesc, nvrhi::CpuAccessMode::Write);
    nvrhi::TextureSlice stagingTextureSlice{};
    stagingTextureSlice.width = image.width;
    stagingTextureSlice.height = image.height;
    size_t pitch;
    const uint32_t actualRowSize = image.data.size() / image.height;
    auto mapPtr = static_cast<uint8_t*>(context.nvrhiDevice->mapStagingTexture(stagingTexture,
                                                                               stagingTextureSlice,
                                                                               nvrhi::CpuAccessMode::Write,
                                                                               &pitch));
    for (int i = 0; i < image.height; ++i) {
        memcpy(mapPtr + (i * pitch),
               image.data.data() + (i * actualRowSize),
               actualRowSize);
    }

    context.nvrhiDevice->unmapStagingTexture(stagingTexture);

    auto commandList = context.nvrhiDevice->createCommandList();

    return std::make_unique<CommandExecution>(context,
                                              commandList,
                                              stagingTexture
        );
}