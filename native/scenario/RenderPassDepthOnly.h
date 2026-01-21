//
// Created by Ingun Jon on 1/21/26.
//

#ifndef NVRHI_UNIT_TEST_RENDERPASSDEPTHONLY_H
#define NVRHI_UNIT_TEST_RENDERPASSDEPTHONLY_H
#include "include/scenario/App.h"


struct RunDepthDrawCommand : public App {
    nvrhi::TextureHandle texture;
    nvrhi::ShaderHandle vertex;
    nvrhi::ShaderHandle pixel;
    nvrhi::FramebufferHandle framebuffer;
    nvrhi::GraphicsPipelineHandle pipeline;

    RunDepthDrawCommand() = delete;

    explicit RunDepthDrawCommand(
        const Context& ctx,
        nvrhi::TextureHandle&& texture,
        nvrhi::ShaderHandle&& vertex,
        nvrhi::ShaderHandle&& pixel,
        nvrhi::FramebufferHandle&& framebuffer,
        nvrhi::GraphicsPipelineHandle&& pipeline
        )
        : App(ctx, "RunDepthDrawCommand", "", ""),
          texture(texture),
          vertex(vertex),
          pixel(pixel),
          framebuffer(framebuffer),
          pipeline(pipeline) {
    }

    AppPtr run(std::string) override;
};

struct RenderPassDepthOnly : public App {
    RenderPassDepthOnly() = delete;

    explicit RenderPassDepthOnly(const Context& ctx)
        : App(ctx, "RenderPassDepthOnly", "", "") {
    }

    AppPtr run(std::string) override;
};


#endif //NVRHI_UNIT_TEST_RENDERPASSDEPTHONLY_H