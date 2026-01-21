//
// Created by Ingun Jon on 1/21/26.
//

#ifndef NVRHI_UNIT_TEST_RENDERPASS_H
#define NVRHI_UNIT_TEST_RENDERPASS_H

#include "include/scenario/App.h"
struct RunDrawCommand : public App {
    nvrhi::TextureHandle texture;
    nvrhi::ShaderHandle vertex;
    nvrhi::ShaderHandle pixel;
    nvrhi::FramebufferHandle framebuffer;
    nvrhi::GraphicsPipelineHandle pipeline;

    RunDrawCommand() = delete;

    explicit RunDrawCommand(
        const Context& ctx,
        nvrhi::TextureHandle&& texture,
        nvrhi::ShaderHandle&& vertex,
        nvrhi::ShaderHandle&& pixel,
        nvrhi::FramebufferHandle&& framebuffer,
        nvrhi::GraphicsPipelineHandle&& pipeline
        )
        : App(ctx, "RunDrawCommand", "", ""),
          texture(texture),
          vertex(vertex),
          pixel(pixel),
          framebuffer(framebuffer),
          pipeline(pipeline) {
    }

    AppPtr run(std::string) override;
};

struct RenderPass : public App {
    RenderPass() = delete;

    explicit RenderPass(const Context& ctx)
        : App(ctx, "RenderPass", "", "") {
    }

    AppPtr run(std::string) override;
};


#endif //NVRHI_UNIT_TEST_RENDERPASS_H