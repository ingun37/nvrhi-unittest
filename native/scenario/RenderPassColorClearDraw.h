//
// Created by Ingun Jon on 1/21/26.
//

#ifndef NVRHI_UNIT_TEST_RENDERPASS_H
#define NVRHI_UNIT_TEST_RENDERPASS_H

#include "include/scenario/App.h"

struct RunDrawCommand : public App {
    nvrhi::TextureHandle colorTexture;
    nvrhi::TextureHandle depthTexture;
    nvrhi::ShaderHandle vertex;
    nvrhi::ShaderHandle pixel;
    nvrhi::FramebufferHandle framebuffer;
    nvrhi::GraphicsPipelineHandle pipeline;

    RunDrawCommand() = delete;

private:
    static std::string prompt() {
        return
            "  First boolean: clear color\n"
            "  Second boolean: clear depth\n"
            "  Third boolean: execute draw command\n"
            "Examples:\n"
            "  true true true    -> clear color/depth and draw\n"
            "  true false true   -> clear color, draw (keep depth)\n"
            "  false true true   -> clear depth, draw (keep color)\n"
            "  true true false   -> clear color/depth only, no draw\n"
            "  false false false -> no clear, no draw";
    }

public:
    explicit RunDrawCommand(
        const Context& ctx,
        nvrhi::TextureHandle&& colorTexture,
        nvrhi::TextureHandle&& depthTexture,
        nvrhi::ShaderHandle&& vertex,
        nvrhi::ShaderHandle&& pixel,
        nvrhi::FramebufferHandle&& framebuffer,
        nvrhi::GraphicsPipelineHandle&& pipeline
        )
        : App(ctx, "RunDrawCommand", prompt(), "true true true"),
          colorTexture(colorTexture),
          depthTexture(depthTexture),
          vertex(vertex),
          pixel(pixel),
          framebuffer(framebuffer),
          pipeline(pipeline) {
    }

    AppPtr run(std::string) override;
};

struct RenderPassColorClearDraw : public App {
    RenderPassColorClearDraw() = delete;

    explicit RenderPassColorClearDraw(const Context& ctx)
        : App(ctx, "RenderPassColorClearDraw", "", "") {
    }

    AppPtr run(std::string) override;
};


#endif //NVRHI_UNIT_TEST_RENDERPASS_H