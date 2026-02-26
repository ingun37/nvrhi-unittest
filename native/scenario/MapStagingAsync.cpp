//
// Created by Ingun Jon on 1/14/26.
//

#include "MapStagingAsync.h"

#include <iostream>
#include <ostream>
#include "Map3DStagingMipMap.h"

namespace {
    struct VerifyStaging : public Step {
        std::vector<uint32_t> staging_content = {};
        nvrhi::StagingTextureHandle stagingTexture;

        VerifyStaging() = delete;

        explicit VerifyStaging(
            const Context &ctx,
            nvrhi::StagingTextureHandle stagingTexture,
            std::vector<uint32_t> staging_content)
            : Step(ctx, "Verify Staging", "", ""),
              stagingTexture(std::move(stagingTexture)),
              staging_content(std::move(staging_content)) {
        }

        StepFuture run(std::string) override {
            auto h = stagingTexture->getDesc().height;
            auto w = stagingTexture->getDesc().width;
            for (int i = 0; i < h; i++) {
                uint32_t base = 1000 * i;
                for (int j = 0; j < w; j++) {
                    assert(staging_content[i * w + j] == base + j);
                }
            }
            return create_null_step();
        }
    };

    struct ReadStaging : public Step {
        nvrhi::StagingTextureHandle stagingTexture;

        ReadStaging() = delete;

        explicit ReadStaging(
            const Context &ctx,
            nvrhi::StagingTextureHandle &&stagingTexture)
            : Step(ctx, "Read Staging", "", ""),
              stagingTexture(std::move(stagingTexture)) {
        }

        StepFuture run(std::string _) override {
            nvrhi::TextureSlice slice{};
            slice = slice.resolve(stagingTexture->getDesc());
            auto rowPitch = std::make_shared<size_t>(0);

            auto staging_content = std::make_shared<std::vector<uint32_t> >();
            StepPromise next_app;
            auto f = next_app.get_future();
            auto next_app_ptr = std::make_shared<StepPromise>(std::move(next_app));

            context.nvrhiDevice->mapStagingTextureAsync(
                stagingTexture,
                slice,
                nvrhi::CpuAccessMode::Read,
                rowPitch.get(),
                [
                    this,
                    rowPitch,
                    staging_content,
                    t = stagingTexture.Get(),
                    next_app_ptr
                ](const void *ptr) mutable {
                    const uint32_t *typed_ptr = static_cast<const uint32_t *>(ptr);
                    std::cout << "callback is called" << std::endl;
                    size_t typed_row_pitch = (*rowPitch) / sizeof(uint32_t);
                    for (size_t i = 0; i < t->getDesc().height; i++) {
                        const uint32_t *row_ptr = typed_ptr + i * typed_row_pitch;
                        staging_content->insert(staging_content->end(),
                                                row_ptr,
                                                row_ptr + t->getDesc().width);
                    }
                    next_app_ptr->set_value(std::make_unique<VerifyStaging>(
                        context,
                        stagingTexture,
                        std::move(*staging_content)));
                    std::cout << "VerifyStaging is created from stagingTexture callback" << std::endl;
                });

            return f;
        }
    };

    struct CopyTextureToStaging : public Step {
        nvrhi::TextureHandle texture;
        nvrhi::StagingTextureHandle stagingTexture;

        CopyTextureToStaging() = delete;

        CopyTextureToStaging(const Context &ctx,
                             nvrhi::TextureHandle &&texture,
                             nvrhi::StagingTextureHandle &&stagingTexture)
            : Step(ctx, "Copy Texture To Staging", "", ""),
              texture(std::move(texture)),
              stagingTexture(std::move(stagingTexture)) {
        }

        StepFuture run(std::string) override {
            auto commandList = context.nvrhiDevice->createCommandList();
            commandList->open();
            commandList->copyTexture(
                stagingTexture,
                nvrhi::TextureSlice{}.resolve(stagingTexture->getDesc()),
                texture,
                nvrhi::TextureSlice{}.resolve(texture->getDesc()));
            commandList->close();
            context.nvrhiDevice->executeCommandList(commandList);
            return create_step_immediately<ReadStaging>(context, std::move(stagingTexture));
        }
    };

    struct WriteTextureAndCreateStaging : public Step {
        nvrhi::TextureHandle texture;

        WriteTextureAndCreateStaging() = delete;

        explicit WriteTextureAndCreateStaging(const Context &ctx,
                                              nvrhi::TextureHandle &&texture
        )
            : Step(ctx, "WriteTextureAndCreateStaging", "", ""),
              texture(std::move(texture)) {
        }

        StepFuture run(std::string _) override {
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
            commandList->close();
            context.nvrhiDevice->executeCommandList(commandList);

            return create_step_immediately<CopyTextureToStaging>(context,
                                                                 std::move(texture),
                                                                 context.nvrhiDevice->createStagingTexture(
                                                                     texture->getDesc(),
                                                                     nvrhi::CpuAccessMode::Read)
            );
        }
    };
}

StepFuture MapStagingAsync::run(std::string _) {
    nvrhi::TextureDesc desc{.width = 200, .height = 2, .depth = 1, .format = nvrhi::Format::RGBA8_UNORM};
    return create_step_immediately<WriteTextureAndCreateStaging>(
        context,
        context.nvrhiDevice->createTexture(desc)
    );
}