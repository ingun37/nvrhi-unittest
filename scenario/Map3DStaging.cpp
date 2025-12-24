//
// Created by Ingun Jon on 12/24/25.
//

#include "Map3DStaging.h"
#include "Image.h"
#include <iostream>

#include "my_io.h"

static nvrhi::TextureHandle create3DTexture(const uint32_t width,
                                            const uint32_t height,
                                            const uint32_t depth,
                                            const nvrhi::Format format,
                                            const nvrhi::DeviceHandle& device) {
    nvrhi::TextureDesc dstTextureDesc{};
    dstTextureDesc.width = width;
    dstTextureDesc.height = height;
    dstTextureDesc.depth = depth;
    dstTextureDesc.dimension = nvrhi::TextureDimension::Texture3D;
    dstTextureDesc.format = format;
    return device->createTexture(dstTextureDesc);
}

static nvrhi::StagingTextureHandle create3DStaging(uint32_t width,
                                                   uint32_t height,
                                                   uint32_t depth,
                                                   const nvrhi::Format format,
                                                   const nvrhi::DeviceHandle& device) {
    nvrhi::TextureDesc stagingTextureDesc{};
    stagingTextureDesc.width = width;
    stagingTextureDesc.height = height;
    stagingTextureDesc.depth = depth;
    stagingTextureDesc.format = format;
    stagingTextureDesc.dimension = nvrhi::TextureDimension::Texture3D;
    return device->createStagingTexture(stagingTextureDesc, nvrhi::CpuAccessMode::Write);
}

struct CommandSimple3DCopy : public App {
    nvrhi::CommandListHandle commandList;
    nvrhi::StagingTextureHandle stagingTexture;

    CommandSimple3DCopy() = delete;

    CommandSimple3DCopy(const Context& webGPU,
                        nvrhi::CommandListHandle commandList,
                        nvrhi::StagingTextureHandle stagingTexture)
        : App(webGPU, "Execute command to copy staged buffer to texture"),
          commandList(std::move(commandList)),
          stagingTexture(std::move(stagingTexture)) {
    }

    AppPtr run() override {
        auto dstTexture = create3DTexture(stagingTexture->getDesc().width,
                                          stagingTexture->getDesc().height,
                                          stagingTexture->getDesc().depth,
                                          stagingTexture->getDesc().format,
                                          context.nvrhiDevice);
        nvrhi::TextureSlice srcTextureSlice = nvrhi::TextureSlice().resolve(stagingTexture->getDesc());
        nvrhi::TextureSlice destSlice = nvrhi::TextureSlice().resolve(stagingTexture->getDesc());

        commandList->open();
        commandList->copyTexture(dstTexture, destSlice, stagingTexture, srcTextureSlice);
        commandList->close();
        context.nvrhiDevice->executeCommandList(commandList);

        return std::make_unique<CommandSimple3DCopy>(*this);
    }
};

AppPtr Map3DStaging::run() {
    std::vector<Image> images;
    const uint32_t depth = 16;
    for (int i = 0; i < depth; ++i) {
        images.push_back(
            Image::load("/Users/ingun/CLionProjects/nvrhi-unit-test/3d-texture/" + std::to_string(i) + ".png"));
    }

    auto sampleImage = images[0];
    std::cout << "Input Slice" << std::endl;
    auto sr = my_io::read_box(0, 0, 0, sampleImage.width, sampleImage.height, images.size());

    auto staging = create3DStaging(sr.width,
                                   sr.height,
                                   sr.depth,
                                   nvrhi::Format::RGBA8_UNORM,
                                   context.nvrhiDevice);

    auto slice = nvrhi::TextureSlice{}.resolve(staging->getDesc());
    slice.width = sr.width;
    slice.height = sr.height;
    size_t pitch;
    auto mapPtr = static_cast<uint8_t*>(context.nvrhiDevice->mapStagingTexture(staging,
                                                                               slice,
                                                                               nvrhi::CpuAccessMode::Write,
                                                                               &pitch));

    const uint32_t imageRowPitch = images[0].data.size() / images[0].height;
    const uint32_t pixelSize = imageRowPitch / images[0].width;

    std::cout << "Pitch: " << pitch << std::endl;
    const uint32_t stagingRowPitch = staging->getDesc().width * pixelSize;
    const uint32_t stagingPlanePitch = stagingRowPitch * staging->getDesc().height;

    for (uint32_t i = 0; i < sr.depth; ++i) {
        for (uint32_t j = 0; j < sr.height; ++j) {
            memcpy(mapPtr + (i * stagingPlanePitch) + (j * stagingRowPitch),
                   images[sr.z + i].data.data() + ((sr.y + j) * imageRowPitch) + (sr.x * pixelSize),
                   pitch);
        }
    }

    context.nvrhiDevice->unmapStagingTexture(staging);

    auto commandList = context.nvrhiDevice->createCommandList();

    return std::make_unique<CommandSimple3DCopy>(context,
                                                 commandList,
                                                 staging);
}