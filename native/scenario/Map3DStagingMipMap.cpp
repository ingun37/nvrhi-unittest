//
// Created by Ingun Jon on 1/12/26.
//

#include "Map3DStagingMipMap.h"
#include "Image.h"
#include <iostream>
#include <ranges>
#include <algorithm>

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


StepFuture Command3DCopyMipMap::run(std::string _) {
    auto dstTexture = create3DTexture(1010,
                                      1010,
                                      4,
                                      stagingTexture->getDesc().format,
                                      context.nvrhiDevice);
    nvrhi::TextureSlice defaultDstSlice = nvrhi::TextureSlice().resolve(dstTexture->getDesc());

    nvrhi::TextureSlice src1Slice = nvrhi::TextureSlice{
            .depth = 2,
            .mipLevel = static_cast<unsigned int>(mipLevel)}.
        resolve(stagingTexture->getDesc());

    nvrhi::TextureSlice dst1Slice(defaultDstSlice);
    dst1Slice.x = 10;
    dst1Slice.y = 10;
    dst1Slice.z = 1;
    dst1Slice.width = src1Slice.width;
    dst1Slice.height = src1Slice.height;
    dst1Slice.depth = 2;

    commandList->open();
    commandList->copyTexture(dstTexture, dst1Slice, stagingTexture, src1Slice);
    commandList->close();
    context.nvrhiDevice->executeCommandList(commandList);

    return create_null_step();
}

StepFuture WriteStagingBuffer::run(std::string _) {
    auto slice = nvrhi::TextureSlice{.mipLevel = static_cast<unsigned int>(mipLevel)}.resolve(staging->getDesc());

    size_t row_pitch;
    auto* mapPtr = static_cast<uint8_t*>(context.nvrhiDevice->mapStagingTexture(staging,
        slice,
        nvrhi::CpuAccessMode::Write,
        &row_pitch));
    std::cout << "mipLevel: " << mipLevel << ", Row pitch: " << row_pitch << std::endl;
    size_t slice_pitch = row_pitch * slice.height;
    for (uint32_t i = 0; i < slice.depth; ++i) {
        for (uint32_t j = 0; j < slice.height; ++j) {
            const Image& image = images[mipLevel][i];
            int image_row_pitch = image.data.size() / image.height;
            memcpy(mapPtr + (i * slice_pitch) + (j * row_pitch),
                   images[mipLevel][i].data.data() + (j * image_row_pitch),
                   row_pitch);
        }
    }
    context.nvrhiDevice->unmapStagingTexture(staging);
    auto commandList = context.nvrhiDevice->createCommandList();

    return create_step_immediately<Command3DCopyMipMap>(context,
                                                        std::move(commandList),
                                                        std::move(staging),
                                                        mipLevel);
}

StepFuture Map3DStagingMipMap::run(std::string _) {
    constexpr int depth = 16;
    constexpr int mipLevels = 2;
    auto images = std::views::iota(0, mipLevels) | std::views::transform([](auto mip) {
        return std::views::iota(0, (depth >> mip)) | std::views::transform([mip](auto z) {
            auto p = std::to_string(mip) + "/" + std::to_string(z * (1 << mip)) + ".png";
            std::cout << "Loading " << p << std::endl;
            return Image::load("/Users/ingun/CLionProjects/nvrhi-unit-test/native/3d-texture/" + p);
        }) | std::ranges::to<std::vector>();
    }) | std::ranges::to<std::vector>();

    auto sample_image = images[0][0];

    nvrhi::TextureDesc stagingTextureDesc{};
    stagingTextureDesc.width = sample_image.width;
    stagingTextureDesc.height = sample_image.height;
    stagingTextureDesc.depth = depth;
    stagingTextureDesc.format = nvrhi::Format::RGBA8_UNORM;
    stagingTextureDesc.mipLevels = 1;
    stagingTextureDesc.dimension = nvrhi::TextureDimension::Texture3D;
    auto staging = this->context.nvrhiDevice->createStagingTexture(stagingTextureDesc, nvrhi::CpuAccessMode::Write);
    int mipLevelInput = 0;
    std::cout << "WebGPU backend doesn't support mipmapping for staging texture. Defaulting to mip level 0.";

    return create_step_immediately<WriteStagingBuffer>(context,
                                                       std::move(staging),
                                                       std::move(images),
                                                       mipLevelInput);
}