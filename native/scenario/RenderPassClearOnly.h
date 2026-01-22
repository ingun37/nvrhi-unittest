//
// Created by Ingun Jon on 1/22/26.
//

#ifndef NVRHI_UNIT_TEST_RENDERPASSCLEARONLY_H
#define NVRHI_UNIT_TEST_RENDERPASSCLEARONLY_H

#include "include/scenario/App.h"

struct RunClearCommand : public App {
    nvrhi::TextureHandle texture;
    nvrhi::ShaderHandle vertex;
    nvrhi::ShaderHandle pixel;
    nvrhi::FramebufferHandle framebuffer;
    nvrhi::GraphicsPipelineHandle pipeline;

    RunClearCommand() = delete;

    explicit RunClearCommand(
        const Context& ctx,
        nvrhi::TextureHandle&& texture,
        nvrhi::ShaderHandle&& vertex,
        nvrhi::ShaderHandle&& pixel,
        nvrhi::FramebufferHandle&& framebuffer,
        nvrhi::GraphicsPipelineHandle&& pipeline
        )
        : App(ctx, "RunClearCommand", "", ""),
          texture(texture),
          vertex(vertex),
          pixel(pixel),
          framebuffer(framebuffer),
          pipeline(pipeline) {
    }

    AppPtr run(std::string) override;
};

struct RenderPassClearOnly : public App {
    RenderPassClearOnly() = delete;

    explicit RenderPassClearOnly(const Context& ctx)
        : App(ctx, "RenderPassClearOnly", "", "") {
    }

    AppPtr run(std::string) override;
};

#endif //NVRHI_UNIT_TEST_RENDERPASSCLEARONLY_H
