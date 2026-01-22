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

private:
    static std::string prompt() {
        return
            "  First boolean: clear framebuffer before drawing\n"
            "  Second boolean: execute draw command\n"
            "Examples:\n"
            "  true true   -> clear and draw\n"
            "  false true  -> draw without clear\n"
            "  true false  -> clear only, no draw\n"
            "  false false -> no clear, no draw";
    }

public:
    explicit RunDrawCommand(
        const Context& ctx,
        nvrhi::TextureHandle&& texture,
        nvrhi::ShaderHandle&& vertex,
        nvrhi::ShaderHandle&& pixel,
        nvrhi::FramebufferHandle&& framebuffer,
        nvrhi::GraphicsPipelineHandle&& pipeline
        )
        : App(ctx, "RunDrawCommand", prompt(), "true true"),
          texture(texture),
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