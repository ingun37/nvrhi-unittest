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


AppPtr Command3DCopyMipMap::run()  {

    auto dstTexture = create3DTexture(1010,
                                      1010,
                                      4,
                                      stagingTexture->getDesc().format,
                                      context.nvrhiDevice);
    nvrhi::TextureSlice defaultDstSlice = nvrhi::TextureSlice().resolve(dstTexture->getDesc());

    nvrhi::TextureSlice dst1Slice(defaultDstSlice);
    dst1Slice.x = 10;
    dst1Slice.y = 10;
    dst1Slice.z = 1;
    dst1Slice.width = stagingTexture->getDesc().width;
    dst1Slice.height = stagingTexture->getDesc().height;
    dst1Slice.depth = 2;

    nvrhi::TextureSlice src1Slice = nvrhi::TextureSlice{.depth=2}.resolve(stagingTexture->getDesc());

    nvrhi::TextureSlice dst2Slice(defaultDstSlice);
    dst2Slice.x = 500;
    dst2Slice.y = 500;
    dst2Slice.z = 2;
    dst2Slice.width = 100;
    dst2Slice.height = 100;
    dst2Slice.depth = 2;

    nvrhi::TextureSlice src2Slice = nvrhi::TextureSlice{.mipLevel = 1,.depth=2}.resolve(stagingTexture->getDesc());

    commandList->open();
    commandList->copyTexture(dstTexture, dst1Slice, stagingTexture, src1Slice);
    commandList->copyTexture(dstTexture, dst2Slice, stagingTexture, src2Slice);
    commandList->close();
    context.nvrhiDevice->executeCommandList(commandList);

    return std::make_unique<Command3DCopyMipMap>(*this);
}

AppPtr Map3DStagingMipMap::run() {
    constexpr int depth = 16;
    constexpr int mipLevels = 2;
    const auto images = std::views::iota(0, mipLevels) | std::views::transform([](auto mip) {
        return std::views::iota(0, (depth >> mip)) | std::views::transform([mip](auto z) {
            auto p = std::to_string(mip) + "/" + std::to_string(z*(1<<mip)) + ".png";
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
    stagingTextureDesc.mipLevels = mipLevels;
    stagingTextureDesc.dimension = nvrhi::TextureDimension::Texture3D;
    auto staging = this->context.nvrhiDevice->createStagingTexture(stagingTextureDesc, nvrhi::CpuAccessMode::Write);

    for (uint32_t mipIdx = 0; mipIdx < mipLevels; ++mipIdx) {


        auto slice = nvrhi::TextureSlice{
            .mipLevel = mipIdx
        }.resolve(staging->getDesc());

        size_t row_pitch;
        auto mapPtr = static_cast<uint8_t*>(context.nvrhiDevice->mapStagingTexture(staging,
                                                                                   slice,
                                                                                   nvrhi::CpuAccessMode::Write,
                                                                                   &row_pitch));
        size_t slice_pitch = row_pitch * slice.height;
        for (uint32_t i = 0; i < slice.depth; ++i) {
            for (uint32_t j = 0; j < slice.height; ++j) {
                memcpy(mapPtr + (i * slice_pitch) + (j * row_pitch),
                       images[mipIdx][i].data.data() + (j * row_pitch),
                       row_pitch);
            }
        }
        context.nvrhiDevice->unmapStagingTexture(staging);
    }




    auto commandList = context.nvrhiDevice->createCommandList();

    return std::make_unique<Command3DCopyMipMap>(context,
                                                 std::move(commandList),
                                                 std::move(staging));
}