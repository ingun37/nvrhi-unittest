//
// Created by Ingun Jon on 1/21/26.
//

#include "RenderPassColorClearDraw.h"

#include <iostream>
#include <fstream>
#include "backend.h"

namespace {
struct Payload {
    nvrhi::TextureHandle colorTexture;
    nvrhi::TextureHandle depthTexture;
    nvrhi::ShaderHandle vertex;
    nvrhi::ShaderHandle pixel;
    nvrhi::FramebufferHandle framebuffer;
    nvrhi::GraphicsPipelineHandle pipeline;
};

struct RunDrawCommand : public Step {
    Payload payload;

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
        Payload&& payload
        )
        : Step(ctx, "RunDrawCommand", prompt(), "true true true"),
          payload(std::move(payload)) {
    }

    StepFuture run(std::string input) override {
        std::istringstream iss(input);
        std::string clearColorStr, clearDepthStr, drawStr;
        iss >> clearColorStr >> clearDepthStr >> drawStr;

        bool shouldClearColor = clearColorStr == "true";
        bool shouldClearDepth = clearDepthStr == "true";
        bool shouldDraw = drawStr == "true";

        auto commandList = context.nvrhiDevice->createCommandList();
        commandList->open();

        if (shouldClearColor) {
            const nvrhi::FramebufferAttachment& att = payload.framebuffer->getDesc().colorAttachments[0];
            std::cout << "Clearing Color..." << std::endl;
            commandList->clearTextureFloat(att.texture, att.subresources, nvrhi::Color(0.1f, 0.3f, 0.6f, 1.0f));
        }

        if (shouldClearDepth) {
            std::cout << "Clearing depth..." << std::endl;
            const nvrhi::FramebufferAttachment& att = payload.framebuffer->getDesc().depthAttachment;

            commandList->clearDepthStencilTexture(att.texture, att.subresources, true, 0.6f, true, 64);
        }

        nvrhi::GraphicsState state;
        state.pipeline = payload.pipeline;
        state.framebuffer = payload.framebuffer;
        state.viewport.addViewportAndScissorRect(payload.framebuffer->getFramebufferInfo().getViewport());
        commandList->setGraphicsState(state);
        nvrhi::DrawArguments args{};
        if (shouldDraw) {
            std::cout << "Drawing..." << std::endl;
            args.vertexCount = 3;
            commandList->draw(args);
        }

        commandList->close();
        context.nvrhiDevice->executeCommandList(commandList);

        return create_null_step();
    }
};
}

std::string padToMultipleOfFour(std::string input) {
    size_t remainder = input.length() % 4;
    if (remainder != 0) {
        size_t paddingNeeded = 4 - remainder;
        input.append(paddingNeeded, ' ');
    }
    return input;
}

StepFuture RenderPassColorClearDraw::run(std::string) {
    // Read data into vector
    auto file_data = read_shader("simple");

    nvrhi::ShaderDesc vertexDesc{};
    vertexDesc.setEntryName("vs");
    vertexDesc.setShaderType(nvrhi::ShaderType::Vertex);
    nvrhi::ShaderDesc pixelDesc{};
    pixelDesc.setEntryName("ps");
    pixelDesc.setShaderType(nvrhi::ShaderType::Pixel);
    auto vertex = context.nvrhiDevice->createShader(vertexDesc, file_data.data(), file_data.size());
    auto pixel = context.nvrhiDevice->createShader(pixelDesc, file_data.data(), file_data.size());

    nvrhi::GraphicsPipelineDesc psoDesc;
    psoDesc.VS = vertex;
    psoDesc.PS = pixel;
    psoDesc.primType = nvrhi::PrimitiveType::TriangleList;
    psoDesc.renderState.depthStencilState.depthTestEnable = false;

    psoDesc.renderState.blendState.targets[0].blendEnable = true;
    psoDesc.renderState.blendState.targets[0].srcBlend = nvrhi::BlendFactor::SrcAlpha;
    psoDesc.renderState.blendState.targets[0].destBlend = nvrhi::BlendFactor::OneMinusSrcAlpha;
    psoDesc.renderState.blendState.targets[0].blendOp = nvrhi::BlendOp::Add;
    psoDesc.renderState.blendState.targets[0].srcBlendAlpha = nvrhi::BlendFactor::One;
    psoDesc.renderState.blendState.targets[0].destBlendAlpha = nvrhi::BlendFactor::OneMinusSrcAlpha;
    psoDesc.renderState.blendState.targets[0].blendOpAlpha = nvrhi::BlendOp::Add;

    nvrhi::TextureDesc colorTextureDesc{};
    colorTextureDesc.setHeight(256);
    colorTextureDesc.setWidth(256);
    colorTextureDesc.setFormat(nvrhi::Format::RGBA8_UNORM);
    colorTextureDesc.setIsRenderTarget(true);
    auto colorTexture = context.nvrhiDevice->createTexture(colorTextureDesc);
    nvrhi::FramebufferAttachment colorAttachment{};
    colorAttachment.setTexture(colorTexture);

    nvrhi::TextureDesc depthTextureDesc{};
    depthTextureDesc.setHeight(256);
    depthTextureDesc.setWidth(256);
    depthTextureDesc.setFormat(nvrhi::Format::D24S8);
    depthTextureDesc.setIsRenderTarget(true);
    auto depthTexture = context.nvrhiDevice->createTexture(depthTextureDesc);

    nvrhi::FramebufferAttachment depthAttachment{};
    depthAttachment.setTexture(depthTexture);

    nvrhi::FramebufferDesc framebufferDesc{};
    framebufferDesc.addColorAttachment(colorAttachment);
    framebufferDesc.setDepthAttachment(depthAttachment);
    auto framebuffer = context.nvrhiDevice->createFramebuffer(framebufferDesc);

    auto pipeline = context.nvrhiDevice->createGraphicsPipeline(psoDesc, framebuffer);

    return create_step_immediately<RunDrawCommand>(
        context,
        Payload{
            std::move(colorTexture),
            std::move(depthTexture),
            std::move(vertex),
            std::move(pixel),
            std::move(framebuffer),
            std::move(pipeline)
        });
}