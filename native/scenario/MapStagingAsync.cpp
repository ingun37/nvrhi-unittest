//
// Created by Ingun Jon on 1/14/26.
//

#include "MapStagingAsync.h"

#include <iostream>
#include <ostream>
#include "Map3DStagingMipMap.h"

AppPtr VerifyStaging::run(std::string) {
    auto h = stagingTexture->getDesc().height;
    auto w = stagingTexture->getDesc().width;
    for (int i = 0; i < h; i++) {
        uint32_t base = 1000 * i;
        for (int j = 0; j < w; j++) {
            assert(staging_content[i * w + j] == base + j);
            assert(buffer_content[i * w + j] == base + j);
        }
    }
    return immediate_null_app();
}

AppPtr ReadStaging::run(std::string _) {
    nvrhi::TextureSlice slice{};
    slice = slice.resolve(stagingTexture->getDesc());
    auto rowPitch = std::make_shared<size_t>(0);

    auto next_app = std::make_unique<VerifyStaging>(context, stagingTexture, buffer);

    context.nvrhiDevice->mapStagingTextureAsync(
        stagingTexture,
        slice,
        nvrhi::CpuAccessMode::Read,
        rowPitch.get(),
        [rowPitch, next_app = next_app.get(), t = stagingTexture.Get()](const void* ptr) {
            const uint32_t* typed_ptr = static_cast<const uint32_t*>(ptr);
            std::cout << "callback is called" << std::endl;
            next_app->staging_content = std::vector<uint32_t>();
            size_t typed_row_pitch = (*rowPitch) / sizeof(uint32_t);
            for (size_t i = 0; i < t->getDesc().height; i++) {
                const uint32_t* row_ptr = typed_ptr + i * typed_row_pitch;
                next_app->staging_content.insert(next_app->staging_content.end(),
                                                 row_ptr,
                                                 row_ptr + t->getDesc().width);
            }
        });
    context.nvrhiDevice->mapBufferAsync(
        buffer,
        nvrhi::CpuAccessMode::Read,
        [next_app = next_app.get(), t = buffer.Get()](const void* ptr) {
            const uint32_t* typed_ptr = static_cast<const uint32_t*>(ptr);
            std::cout << "callback is called" << std::endl;
            next_app->buffer_content = std::vector<uint32_t>();
            next_app->buffer_content.insert(next_app->buffer_content.end(),
                                            typed_ptr,
                                            typed_ptr + t->getDesc().byteSize);
        });
    return immediate_app(std::move(next_app));
}

AppPtr CopyTextureToStaging::run(std::string) {
    auto commandList = context.nvrhiDevice->createCommandList();
    commandList->open();
    commandList->copyTexture(
        stagingTexture,
        nvrhi::TextureSlice{}.resolve(stagingTexture->getDesc()),
        texture,
        nvrhi::TextureSlice{}.resolve(texture->getDesc()));
    commandList->close();
    context.nvrhiDevice->executeCommandList(commandList);
    return immediate_app(std::make_unique<ReadStaging>(context, std::move(stagingTexture), std::move(buffer)));
}

AppPtr WriteTextureAndCreateStaging::run(std::string _) {
    nvrhi::TextureSlice slice{};
    slice = slice.resolve(texture->getDesc());

    std::vector<uint32_t> data;
    data.reserve(slice.height * slice.width);
    for (size_t i = 0; i < slice.height; i++) {
        for (size_t j = 0; j < slice.width; j++) {
            data.emplace_back(static_cast<uint32_t>(i * 1000 + j));
        }
    }

    auto commandList = context.nvrhiDevice->createCommandList();
    commandList->open();
    auto row_pitch = slice.width * sizeof(uint32_t);
    auto slice_pitch = row_pitch * slice.height;
    commandList->writeTexture(
        texture,
        0,
        0,
        data.data(),
        row_pitch,
        slice_pitch
        );
    commandList->writeBuffer(buffer, data.data(), data.size() * sizeof(uint32_t));
    commandList->close();
    context.nvrhiDevice->executeCommandList(commandList);

    return immediate_app(std::make_unique<CopyTextureToStaging>(context,
                                                                std::move(texture),
                                                                std::move(buffer),
                                                                context.nvrhiDevice->createStagingTexture(
                                                                    texture->getDesc(),
                                                                    nvrhi::CpuAccessMode::Read)
        ));
}

AppPtr MapStagingAsync::run(std::string _) {
    nvrhi::TextureDesc desc{.width = 200, .height = 2, .depth = 1, .format = nvrhi::Format::RGBA8_UNORM};
    nvrhi::BufferDesc bd{.byteSize = 200 * 2 * 4, .cpuAccess = nvrhi::CpuAccessMode::Read};
    return immediate_app(std::make_unique<WriteTextureAndCreateStaging>(
        context,
        context.nvrhiDevice->createTexture(desc),
        context.nvrhiDevice->createBuffer(bd)
        ));
}