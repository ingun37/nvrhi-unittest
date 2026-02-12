//
// Created by Ingun Jon on 1/12/26.
//

#ifndef NVRHI_UNIT_TEST_MAP3DSTAGINGMIPMAP_H
#define NVRHI_UNIT_TEST_MAP3DSTAGINGMIPMAP_H


#include "include/scenario/Step.h"
#include "Image.h"

struct Command3DCopyMipMap : public Step {
    nvrhi::CommandListHandle commandList;
    nvrhi::StagingTextureHandle stagingTexture;
    int mipLevel;

    Command3DCopyMipMap() = delete;

    Command3DCopyMipMap(const Context &webGPU,
                        nvrhi::CommandListHandle &&commandList,
                        nvrhi::StagingTextureHandle &&stagingTexture,
                        int mipLevel)
        : Step(webGPU, "[Command3DCopyMipMap] copy staging to texture ", "", ""),
          commandList(std::move(commandList)),
          stagingTexture(std::move(stagingTexture)),
          mipLevel(mipLevel) {
    }

    StepFuture run(std::string _) override;
};

struct WriteStagingBuffer : public Step {
    nvrhi::StagingTextureHandle staging;
    const std::vector<std::vector<Image> > images;
    int mipLevel;

    WriteStagingBuffer() = delete;

    WriteStagingBuffer(const Context &ctx,
                       nvrhi::StagingTextureHandle &&hdl,
                       const std::vector<std::vector<Image> > &&images,
                       int mipLevel)
        : Step(ctx, "Write Staging Texture", "", ""),
          staging(std::move(hdl)),
          images(std::move(images)),
          mipLevel(mipLevel) {
    };

    StepFuture run(std::string) override;
};

struct Map3DStagingMipMap : public Step {
    Map3DStagingMipMap() = delete;

    Map3DStagingMipMap(const Context &ctx)
        : Step(ctx, "Map 3D Staging Texture MipMap", "", "") {
    }

    StepFuture run(std::string) override;
};

#endif //NVRHI_UNIT_TEST_MAP3DSTAGINGMIPMAP_H