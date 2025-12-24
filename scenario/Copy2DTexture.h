//
// Created by Ingun Jon on 12/24/25.
//

#ifndef NVRHI_UNIT_TEST_COPY2DTEXTURE_H
#define NVRHI_UNIT_TEST_COPY2DTEXTURE_H
#include "App.h"
#include <nvrhi/nvrhi.h>
#include <ios>
#include <Image.h>

struct CommandExecution : public App {
    nvrhi::CommandListHandle commandList;
    nvrhi::TextureHandle destTexture;
    nvrhi::StagingTextureHandle stagingTexture;

    CommandExecution() = delete;

    CommandExecution(const Context& webGPU,
                     nvrhi::CommandListHandle commandList,
                     nvrhi::TextureHandle destTexture,
                     nvrhi::StagingTextureHandle stagingTexture);

    AppPtr run() override;
};

struct ResourceSetup : public App {
    Image image;
    uint32_t dest_x;

    ResourceSetup(Image image, const Context& webGPU, uint32_t dest_x);

    AppPtr run() override;
};

struct ImageLoading : public App {
    ImageLoading() = delete;

    ImageLoading(const Context& webGpu);

    AppPtr run() override;
};


#endif //NVRHI_UNIT_TEST_COPY2DTEXTURE_H