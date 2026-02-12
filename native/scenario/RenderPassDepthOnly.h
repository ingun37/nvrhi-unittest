//
// Created by Ingun Jon on 1/21/26.
//

#ifndef NVRHI_UNIT_TEST_RENDERPASSDEPTHONLY_H
#define NVRHI_UNIT_TEST_RENDERPASSDEPTHONLY_H
#include "include/scenario/Step.h"


struct RunDepthDrawCommand : public Step {
    nvrhi::TextureHandle texture;
    nvrhi::ShaderHandle vertex;
    nvrhi::ShaderHandle pixel;
    nvrhi::FramebufferHandle framebuffer;
    nvrhi::GraphicsPipelineHandle pipeline;

    RunDepthDrawCommand() = delete;

    explicit RunDepthDrawCommand(
        const Context &ctx,
        nvrhi::TextureHandle &&texture,
        nvrhi::ShaderHandle &&vertex,
        nvrhi::ShaderHandle &&pixel,
        nvrhi::FramebufferHandle &&framebuffer,
        nvrhi::GraphicsPipelineHandle &&pipeline
    )
        : Step(ctx, "RunDepthDrawCommand", "", ""),
          texture(texture),
          vertex(vertex),
          pixel(pixel),
          framebuffer(framebuffer),
          pipeline(pipeline) {
    }

    StepFuture run(std::string) override;
};

struct RenderPassDepthOnly : public Step {
    RenderPassDepthOnly() = delete;

    explicit RenderPassDepthOnly(const Context &ctx)
        : Step(ctx, "RenderPassDepthOnly", "", "") {
    }

    StepFuture run(std::string) override;
};


#endif //NVRHI_UNIT_TEST_RENDERPASSDEPTHONLY_H