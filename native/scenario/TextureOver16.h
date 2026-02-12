//
// Created by Ingun Jon on 2/9/26.
//

#ifndef NVRHI_UNIT_TEST_TEXTUREOVER16_H
#define NVRHI_UNIT_TEST_TEXTUREOVER16_H
#include <utility>

#include "include/scenario/Step.h"

struct TextureOver16Draw : public Step {
    nvrhi::TextureHandle colorTexture;
    nvrhi::ShaderHandle vertex;
    nvrhi::ShaderHandle pixel;
    nvrhi::FramebufferHandle framebuffer;
    std::vector<nvrhi::TextureHandle> textures;
    nvrhi::CommandListHandle commandList;

    TextureOver16Draw() = delete;

    explicit TextureOver16Draw(const Context &ctx,
                               nvrhi::TextureHandle &&colorTexture,
                               nvrhi::ShaderHandle &&vertex,
                               nvrhi::ShaderHandle &&pixel,
                               nvrhi::FramebufferHandle &&framebuffer,
                               nvrhi::CommandListHandle &&commandList,
                               std::vector<nvrhi::TextureHandle> textures

    )
        : Step(ctx, "RunDrawCommand", "", ""),
          colorTexture(colorTexture),
          vertex(vertex),
          pixel(pixel),
          framebuffer(framebuffer),
          commandList(commandList),
          textures(std::move(textures)) {
    }

    StepFuture run(std::string) override;
};

struct TextureOver16 : public Step {
    TextureOver16() = delete;

    explicit TextureOver16(const Context &ctx)
        : Step(ctx, "TextureOver16", "", "") {
    }

    StepFuture run(std::string) override;
};


#endif //NVRHI_UNIT_TEST_TEXTUREOVER16_H