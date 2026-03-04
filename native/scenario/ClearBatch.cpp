//
// Created by Ingun Jon on 3/3/26.
//
#include<vector>
#include "ClearBatch.h"

#include <iostream>

#include "backend.h"
#include <ranges>
#include <sstream>
#include <list>

namespace {
struct Payload {
    std::vector<nvrhi::TextureHandle> colorTextures;
    std::vector<nvrhi::TextureHandle> depthTextures;
    nvrhi::ShaderHandle vertex;
    nvrhi::ShaderHandle pixel;
    nvrhi::FramebufferHandle framebuffer;
    nvrhi::GraphicsPipelineHandle pipeline;
};

struct Draw : public Step {
    Payload payload;

    Draw() = delete;


    explicit Draw(const Context& ctx, Payload&& payload)
        : Step(ctx, "Draw", "clear depth count. -1 is all", "-1"),
          payload(std::move(payload)) {
    }

    StepFuture run(std::string input) override {
        std::stringstream ss(input);
        int clearDepthCnt = 0;
        ss >> clearDepthCnt;

        if (clearDepthCnt < 0) clearDepthCnt = payload.depthTextures.size();
        std::cout << "Clearing " << clearDepthCnt << " depth textures" << std::endl;
        auto commandList = context.nvrhiDevice->createCommandList();
        commandList->open();
        for (const auto& texture : payload.colorTextures) {
            commandList->clearTextureFloat(texture,
                                           nvrhi::TextureSubresourceSet(0, 1, 0, 1),
                                           nvrhi::Color(0.1f, 0.3f, 0.6f, 1.0f));
        }

        for (const auto& texture : payload.depthTextures | std::views::take(clearDepthCnt)) {
            commandList->clearDepthStencilTexture(texture,
                                                  nvrhi::TextureSubresourceSet(0, 1, 0, 1),
                                                  true,
                                                  0,
                                                  false,
                                                  0);
        }
        nvrhi::GraphicsState state;
        state.pipeline = payload.pipeline;
        state.framebuffer = payload.framebuffer;
        state.viewport.addViewportAndScissorRect(payload.framebuffer->getFramebufferInfo().getViewport());
        commandList->setGraphicsState(state);
        nvrhi::DrawArguments args{};
        args.vertexCount = 3;
        commandList->draw(args);

        commandList->close();
        context.nvrhiDevice->executeCommandList(commandList);
        return create_null_step();
    }
};
}

StepFuture ClearBatch::run(std::string) {
    auto file_data = read_shader("multi-target");
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
    psoDesc.renderState.depthStencilState.depthWriteEnable = true;

    nvrhi::TextureDesc colorTextureDesc{};
    colorTextureDesc.setHeight(16);
    colorTextureDesc.setWidth(16);
    colorTextureDesc.setFormat(nvrhi::Format::RGBA8_UNORM);
    colorTextureDesc.setIsRenderTarget(true);

    auto colorTextures = std::views::iota(0, 18) | std::views::transform([&colorTextureDesc, this](auto i) {
        colorTextureDesc.debugName = "colorTexture" + std::to_string(i);
        return context.nvrhiDevice->createTexture(colorTextureDesc);
    }) | std::ranges::to<std::vector>();
    nvrhi::TextureDesc depthTextureDesc{};
    depthTextureDesc.setHeight(16);
    depthTextureDesc.setWidth(16);
    depthTextureDesc.setFormat(nvrhi::Format::D32);
    depthTextureDesc.setIsRenderTarget(true);
    auto depthTextures = std::views::iota(0, 4) | std::views::transform([&depthTextureDesc, this](auto i) {
        depthTextureDesc.debugName = "depthTexture" + std::to_string(i);
        return context.nvrhiDevice->createTexture(depthTextureDesc);
    }) | std::ranges::to<std::vector>();
    nvrhi::FramebufferDesc framebufferDesc{};
    {
        nvrhi::FramebufferAttachment depthAttachment{};
        depthAttachment.setTexture(depthTextures.front());
        framebufferDesc.setDepthAttachment(depthAttachment);
    }
    for (auto texture : colorTextures | std::views::take(4)) {
        nvrhi::FramebufferAttachment colorAttachment{};
        colorAttachment.setTexture(texture);
        framebufferDesc.addColorAttachment(colorAttachment);
    }

    auto framebuffer = context.nvrhiDevice->createFramebuffer(framebufferDesc);

    auto pipeline = context.nvrhiDevice->createGraphicsPipeline(psoDesc, framebuffer);
    return create_step_immediately<Draw>(context,
                                         Payload{std::move(colorTextures), std::move(depthTextures), std::move(vertex),
                                                 std::move(pixel), std::move(framebuffer), std::move(pipeline)});
}