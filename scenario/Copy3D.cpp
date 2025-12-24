//
// Created by Ingun Jon on 12/24/25.
//

#include "Copy3D.h"

#include <iostream>
#include <ostream>

#include "Image.h"


nvrhi::TextureHandle createRegular2DTexture(const uint32_t width,
                                            const uint32_t height,
                                            const nvrhi::Format format,
                                            const nvrhi::DeviceHandle& device) {
    nvrhi::TextureDesc dstTextureDesc{};
    dstTextureDesc.width = width;
    dstTextureDesc.height = height;
    dstTextureDesc.format = format;
    return device->createTexture(dstTextureDesc);
}

struct CommandCopy : public App {
    nvrhi::CommandListHandle commandList;
    nvrhi::StagingTextureHandle stagingTexture;

    CommandCopy() = delete;

    CommandCopy(const Context& webGPU,
                nvrhi::CommandListHandle commandList,
                nvrhi::StagingTextureHandle stagingTexture)
        : App(webGPU, "Execute command to copy staged buffer to texture"),
          commandList(std::move(commandList)),
          stagingTexture(std::move(stagingTexture)) {
    }

    AppPtr run() override {
        const uint32_t width = stagingTexture->getDesc().width;
        const uint32_t height = stagingTexture->getDesc().height;
        auto dstTexture = createRegular2DTexture(width * 2,
                                                 height * 2,
                                                 stagingTexture->getDesc().format,
                                                 context.nvrhiDevice);
        nvrhi::TextureSlice srcTextureSlice = nvrhi::TextureSlice().resolve(stagingTexture->getDesc());
        srcTextureSlice.z = 10;
        nvrhi::TextureSlice destSlice{};
        destSlice.x = 120;
        destSlice.y = 300;
        destSlice.width = static_cast<uint32_t>(width * 0.75);
        destSlice.height = static_cast<uint32_t>(height * 0.333);
        destSlice.depth = 1;

        commandList->open();
        commandList->copyTexture(dstTexture, destSlice, stagingTexture, srcTextureSlice);
        commandList->close();
        context.nvrhiDevice->executeCommandList(commandList);

        return std::make_unique<CommandCopy>(*this);
    }
};

AppPtr Copy3D::run() {
    std::vector<Image> images;
    const uint32_t depth = 16;
    for (int i = 0; i < depth; ++i) {
        images.push_back(
            Image::load("/Users/ingun/CLionProjects/nvrhi-unit-test/3d-texture/" + std::to_string(i) + ".png"));
    }

    nvrhi::TextureDesc stagingTextureDesc{};
    const uint32_t width = images[0].width;
    const uint32_t height = images[0].height;
    stagingTextureDesc.width = width;
    stagingTextureDesc.height = height;
    stagingTextureDesc.format = nvrhi::Format::RGBA8_UNORM;
    stagingTextureDesc.depth = depth;
    stagingTextureDesc.mipLevels = 1;
    stagingTextureDesc.arraySize = 1;
    stagingTextureDesc.dimension = nvrhi::TextureDimension::Texture3D;

    auto stagingTexture = context.nvrhiDevice->
        createStagingTexture(stagingTextureDesc, nvrhi::CpuAccessMode::Write);

    nvrhi::TextureSlice _slice{};
    auto stagingTextureSlice = _slice.resolve(stagingTextureDesc);
    size_t pitch;
    const uint32_t actualRowSize = images[0].data.size() / height;
    auto mapPtr = static_cast<uint8_t*>(context.nvrhiDevice->mapStagingTexture(stagingTexture,
                                                                               stagingTextureSlice,
                                                                               nvrhi::CpuAccessMode::Write,
                                                                               &pitch));
    std::cout << "Pitch: " << pitch << std::endl;
    for (int i = 0; i < depth; ++i)
        for (int j = 0; j < height; ++j) {
            memcpy(mapPtr + (i * pitch * height) + (j * pitch),
                   images[i].data.data() + (j * actualRowSize),
                   actualRowSize);
        }
    context.nvrhiDevice->unmapStagingTexture(stagingTexture);

    auto commandList = context.nvrhiDevice->createCommandList();
    return std::make_unique<CommandCopy>(context,
                                         commandList,
                                         stagingTexture);
}