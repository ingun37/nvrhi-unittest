//
// Created by Ingun Jon on 12/24/25.
//

#include "Map3DStaging.h"
#include "Image.h"
#include <iostream>

#include "my_io.h"
#include "nvrhi_util.h"

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


struct Command3DCopyMipMap : public App {
    nvrhi::CommandListHandle commandList;
    nvrhi::StagingTextureHandle stagingTexture;

    Command3DCopyMipMap() = delete;

    Command3DCopyMipMap(const Context& webGPU,
                        nvrhi::CommandListHandle commandList,
                        nvrhi::StagingTextureHandle stagingTexture)
        : App(webGPU, "Execute command to copy staged buffer to texture"),
          commandList(std::move(commandList)),
          stagingTexture(std::move(stagingTexture)) {
    }

    AppPtr run() override {
        std::cout << "Input Destination Texture size. Default is x2" << std::endl;
        auto dstSize = my_io::read_Extent3D(stagingTexture->getDesc().width * 2,
                                            stagingTexture->getDesc().height * 2,
                                            stagingTexture->getDesc().depth * 2);

        auto dstTexture = create3DTexture(dstSize.width,
                                          dstSize.height,
                                          dstSize.depth,
                                          stagingTexture->getDesc().format,
                                          context.nvrhiDevice);
        nvrhi::TextureSlice defaultDstSlice = nvrhi::TextureSlice().resolve(dstTexture->getDesc());

        std::cout <<
            "Input Destination Texture slice origin. Default is (dst.size - src.size)/2. Size is set to src staging texture size. You may want to start capturing from Metal debugger here."
            << std::endl;
        auto dstOrigin = my_io::read_Origin3D((defaultDstSlice.width - stagingTexture->getDesc().width) / 2,
                                              (defaultDstSlice.height - stagingTexture->getDesc().height) / 2,
                                              (defaultDstSlice.depth - stagingTexture->getDesc().depth) / 2);
        nvrhi::TextureSlice dstSlice(defaultDstSlice);
        dstSlice.x = dstOrigin.x;
        dstSlice.y = dstOrigin.y;
        dstSlice.z = dstOrigin.z;
        dstSlice.width = stagingTexture->getDesc().width;
        dstSlice.height = stagingTexture->getDesc().height;
        dstSlice.depth = stagingTexture->getDesc().depth;

        nvrhi::TextureSlice srcSlice = nvrhi::TextureSlice().resolve(stagingTexture->getDesc());

        commandList->open();
        commandList->copyTexture(dstTexture, dstSlice, stagingTexture, srcSlice);
        commandList->close();
        context.nvrhiDevice->executeCommandList(commandList);

        return std::make_unique<Command3DCopyMipMap>(*this);
    }
};

AppPtr Map3DStaging::run() {
    std::vector<Image> images;
    const uint32_t depth = 16;
    for (int i = 0; i < depth; ++i) {
        images.push_back(
            Image::load("/Users/ingun/CLionProjects/nvrhi-unit-test/native/3d-texture/" + std::to_string(i) + ".png"));
    }

    auto sampleImage = images[0];
    std::cout << "Input Slice" << std::endl;
    auto sr = my_io::read_box(100, 130, 11, 120, 80, 2);

    auto staging = nvrhi_util::create3DStaging(sr.width,
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

    return std::make_unique<Command3DCopyMipMap>(context,
                                                 commandList,
                                                 staging);
}