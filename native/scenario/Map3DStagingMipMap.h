//
// Created by Ingun Jon on 1/12/26.
//

#ifndef NVRHI_UNIT_TEST_MAP3DSTAGINGMIPMAP_H
#define NVRHI_UNIT_TEST_MAP3DSTAGINGMIPMAP_H



#include "App.h"

struct Command3DCopyMipMap : public App {
    nvrhi::CommandListHandle commandList;
    nvrhi::StagingTextureHandle stagingTexture;

    Command3DCopyMipMap() = delete;

    Command3DCopyMipMap(const Context& webGPU,
                        nvrhi::CommandListHandle&& commandList,
                        nvrhi::StagingTextureHandle&& stagingTexture)
        : App(webGPU, "[Command3DCopyMipMap] copy staging to texture "),
          commandList(std::move(commandList)),
          stagingTexture(std::move(stagingTexture)) {
    }

    AppPtr run() override ;
};

struct Map3DStagingMipMap : public App {
    Map3DStagingMipMap() = delete;

    Map3DStagingMipMap(const Context& ctx)
        : App(ctx, "Map 3D Staging Texture MipMap") {
    }

    AppPtr run() override;
};

#endif //NVRHI_UNIT_TEST_MAP3DSTAGINGMIPMAP_H
