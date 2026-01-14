//
// Created by Ingun Jon on 1/14/26.
//

#ifndef NVRHI_UNIT_TEST_MAPSTAGINGASYNC_H
#define NVRHI_UNIT_TEST_MAPSTAGINGASYNC_H
#include "App.h"

struct VerifyStaging : public App {
    std::vector<uint32_t> staging_content = {};
    std::vector<uint32_t> buffer_content = {};
    nvrhi::StagingTextureHandle stagingTexture;
    nvrhi::BufferHandle buffer;

    VerifyStaging() = delete;

    explicit VerifyStaging(
        const Context& ctx,
        nvrhi::StagingTextureHandle stagingTexture,
        nvrhi::BufferHandle buffer)
        : App(ctx, "Verify Staging", "", ""),
          stagingTexture(std::move(stagingTexture)),
          buffer(std::move(buffer)) {
    }

    AppPtr run(std::string) override;
};

struct ReadStaging : public App {
    nvrhi::StagingTextureHandle stagingTexture;
    nvrhi::BufferHandle buffer;

    ReadStaging() = delete;

    explicit ReadStaging(
        const Context& ctx,
        nvrhi::StagingTextureHandle&& stagingTexture,
        nvrhi::BufferHandle&& buffer)
        : App(ctx, "Read Staging", "", ""),
          stagingTexture(std::move(stagingTexture)),
          buffer(buffer) {
    }

    AppPtr run(std::string) override;
};

struct CopyTextureToStaging : public App {
    nvrhi::TextureHandle texture;
    nvrhi::BufferHandle buffer;
    nvrhi::StagingTextureHandle stagingTexture;

    CopyTextureToStaging() = delete;

    CopyTextureToStaging(const Context& ctx,
                         nvrhi::TextureHandle&& texture,
                         nvrhi::BufferHandle&& buffer,
                         nvrhi::StagingTextureHandle&& stagingTexture)
        : App(ctx, "Copy Texture To Staging", "", ""),
          texture(std::move(texture)),
          buffer(std::move(buffer)),
          stagingTexture(std::move(stagingTexture)) {
    }

    AppPtr run(std::string) override;
};

struct WriteTextureAndCreateStaging : public App {
    nvrhi::TextureHandle texture;
    nvrhi::BufferHandle buffer;

    WriteTextureAndCreateStaging() = delete;

    explicit WriteTextureAndCreateStaging(const Context& ctx,
                                          nvrhi::TextureHandle&& texture,
                                          nvrhi::BufferHandle&& buffer
        )
        : App(ctx, "WriteTextureAndCreateStaging", "", ""),
          texture(std::move(texture)),
          buffer(std::move(buffer)) {
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