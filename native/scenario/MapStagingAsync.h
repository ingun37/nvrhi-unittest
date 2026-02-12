//
// Created by Ingun Jon on 1/14/26.
//

#ifndef NVRHI_UNIT_TEST_MAPSTAGINGASYNC_H
#define NVRHI_UNIT_TEST_MAPSTAGINGASYNC_H
#include "include/scenario/Step.h"

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

    StepFuture run(std::string) override;
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

    StepFuture run(std::string) override;
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

    StepFuture run(std::string) override;
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

    StepFuture run(std::string) override;
};

struct MapStagingAsync : public Step {
    MapStagingAsync() = delete;

    explicit MapStagingAsync(const Context &ctx)
        : Step(ctx, "Map Staging Async", "", "") {
    }

    StepFuture run(std::string) override;
};


#endif //NVRHI_UNIT_TEST_MAPSTAGINGASYNC_H