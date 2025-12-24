//
// Created by Ingun Jon on 12/24/25.
//

#include "Copy2DTexture.h"
#include <iostream>
#include <sstream>

nvrhi::Format format(const Image& img) {
    if (img.channel == 4) return nvrhi::Format::RGBA8_UNORM;
    throw std::runtime_error("invalid image channel");
}

CommandExecution::CommandExecution(const Context& webGPU,
                                   nvrhi::CommandListHandle commandList,
                                   nvrhi::TextureHandle destTexture,
                                   nvrhi::StagingTextureHandle stagingTexture)
    : App(webGPU, "Execute command to copy staged buffer to texture"),
      commandList(std::move(commandList)),
      destTexture(std::move(destTexture)),
      stagingTexture(std::move(stagingTexture)) {
}

AppPtr CommandExecution::run() {
    std::cout <<
        "Enter destination offset_x, offset_y, width_ratio, and height_ratio (space-separated, default: 0 0 0.5 0.5): ";
    std::string input;
    std::getline(std::cin, input);
    uint32_t dst_offset_x = 0;
    uint32_t dst_offset_y = 0;
    float dst_width_ratio = 0.5f;
    float dst_height_ratio = 0.5f;

    if (!input.empty()) {
        std::istringstream iss(input);
        uint32_t temp_ox, temp_oy;
        float temp_width, temp_height;
        if (iss >> temp_ox >> temp_oy >> temp_width >> temp_height) {
            dst_offset_x = temp_ox;
            dst_offset_y = temp_oy;
            dst_width_ratio = temp_width;
            dst_height_ratio = temp_height;
        }
    }

    uint32_t original_width = stagingTexture->getDesc().width;
    uint32_t original_height = stagingTexture->getDesc().height;
    nvrhi::TextureSlice destSlice{};
    destSlice.x = dst_offset_x;
    destSlice.y = dst_offset_y;
    destSlice.width = static_cast<uint32_t>(original_width * dst_width_ratio);
    destSlice.height = static_cast<uint32_t>(original_height * dst_height_ratio);
    destSlice.depth = 1;
    nvrhi::TextureSlice srcTextureSlice{};
    srcTextureSlice.x = 0;
    srcTextureSlice.y = 0;
    srcTextureSlice.width = original_width;
    srcTextureSlice.height = original_height;
    srcTextureSlice.depth = 1;
    commandList->open();
    commandList->copyTexture(destTexture, destSlice, stagingTexture, srcTextureSlice);
    commandList->close();
    context.nvrhiDevice->executeCommandList(commandList);
    return std::make_unique<CommandExecution>(context,
                                              commandList,
                                              destTexture,
                                              stagingTexture
        );
}

ResourceSetup::ResourceSetup(Image image, const Context& webGPU, uint32_t dest_x)
    : App(webGPU, "Set up textuere reosurces and copy image to staged buffer"),
      image(std::move(image)),
      dest_x(dest_x) {
}

AppPtr ResourceSetup::run() {
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
    nvrhi::TextureDesc destTextureDesc{};
    destTextureDesc.width = image.width * 2;
    destTextureDesc.height = image.height * 2;
    destTextureDesc.format = format(image);
    auto destTexture = context.nvrhiDevice->createTexture(destTextureDesc);

    auto commandList = context.nvrhiDevice->createCommandList();

    return std::make_unique<CommandExecution>(context,
                                              commandList,
                                              destTexture,
                                              stagingTexture
        );
}

ImageLoading::ImageLoading(const Context& webGpu)
    : App(webGpu, "Load image from file") {
}

AppPtr ImageLoading::run() {
    return std::make_unique<ResourceSetup>(
        Image::load("/Users/ingun/CLionProjects/nvrhi-unit-test/uv_grid_opengl_small_remainder.png"),
        context,
        256);
}