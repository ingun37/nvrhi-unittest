//
// Created by Ingun Jon on 1/12/26.
//

#ifndef NVRHI_UNIT_TEST_MAP3DSTAGINGMIPMAP_H
#define NVRHI_UNIT_TEST_MAP3DSTAGINGMIPMAP_H



#include "include/scenario/App.h"
#include "Image.h"

struct Command3DCopyMipMap : public App {
    nvrhi::CommandListHandle commandList;
    nvrhi::StagingTextureHandle stagingTexture;
int mipLevel;

    Command3DCopyMipMap() = delete;

    Command3DCopyMipMap(const Context& webGPU,
                        nvrhi::CommandListHandle&& commandList,
                        nvrhi::StagingTextureHandle&& stagingTexture,
                        int mipLevel)
        : App(webGPU, "[Command3DCopyMipMap] copy staging to texture ", "", ""),
          commandList(std::move(commandList)),
          stagingTexture(std::move(stagingTexture)),
          mipLevel(mipLevel) {
    }

    AppPtr run(std::string _) override;
};

struct WriteStagingBuffer : public App {
    nvrhi::StagingTextureHandle staging;
    const std::vector<std::vector<Image> > images;
    int mipLevel;

    WriteStagingBuffer() = delete;

    WriteStagingBuffer(const Context& ctx,
                       nvrhi::StagingTextureHandle&& hdl,
                       const std::vector<std::vector<Image> >&& images,
                       int mipLevel)
        : App(ctx, "Write Staging Texture", "", ""),
          staging(std::move(hdl)),
          images(std::move(images)),
          mipLevel(mipLevel) {
    };

    AppPtr run(std::string) override;
};

struct Map3DStagingMipMap : public App {
    Map3DStagingMipMap() = delete;

    Map3DStagingMipMap(const Context& ctx)
        : App(ctx, "Map 3D Staging Texture MipMap", "", "") {
    }

    AppPtr run(std::string) override;
};

#endif //NVRHI_UNIT_TEST_MAP3DSTAGINGMIPMAP_H
