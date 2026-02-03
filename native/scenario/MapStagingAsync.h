//
// Created by Ingun Jon on 1/14/26.
//

#ifndef NVRHI_UNIT_TEST_MAPSTAGINGASYNC_H
#define NVRHI_UNIT_TEST_MAPSTAGINGASYNC_H
#include "include/scenario/App.h"

struct VerifyStaging : public App {
    std::vector<uint32_t> staging_content = {};
    nvrhi::StagingTextureHandle stagingTexture;

    VerifyStaging() = delete;

    explicit VerifyStaging(
        const Context& ctx,
        nvrhi::StagingTextureHandle stagingTexture,
        std::vector<uint32_t> staging_content)
        : App(ctx, "Verify Staging", "", ""),
          stagingTexture(std::move(stagingTexture)),
          staging_content(std::move(staging_content)) {
    }

    AppPtr run(std::string) override;
};

struct ReadStaging : public App {
    nvrhi::StagingTextureHandle stagingTexture;

    ReadStaging() = delete;

    explicit ReadStaging(
        const Context& ctx,
        nvrhi::StagingTextureHandle&& stagingTexture)
        : App(ctx, "Read Staging", "", ""),
          stagingTexture(std::move(stagingTexture)) {
    }

    AppPtr run(std::string) override;
};

struct CopyTextureToStaging : public App {
    nvrhi::TextureHandle texture;
    nvrhi::StagingTextureHandle stagingTexture;

    CopyTextureToStaging() = delete;

    CopyTextureToStaging(const Context& ctx,
                         nvrhi::TextureHandle&& texture,
                         nvrhi::StagingTextureHandle&& stagingTexture)
        : App(ctx, "Copy Texture To Staging", "", ""),
          texture(std::move(texture)),
          stagingTexture(std::move(stagingTexture)) {
    }

    AppPtr run(std::string) override;
};

struct WriteTextureAndCreateStaging : public App {
    nvrhi::TextureHandle texture;

    WriteTextureAndCreateStaging() = delete;

    explicit WriteTextureAndCreateStaging(const Context& ctx,
                                          nvrhi::TextureHandle&& texture
        )
        : App(ctx, "WriteTextureAndCreateStaging", "", ""),
          texture(std::move(texture)) {
    }

    AppPtr run(std::string) override;
};

struct MapStagingAsync : public App {
    MapStagingAsync() = delete;

    explicit MapStagingAsync(const Context& ctx)
        : App(ctx, "Map Staging Async", "", "") {
    }

    AppPtr run(std::string) override;
};


#endif //NVRHI_UNIT_TEST_MAPSTAGINGASYNC_H