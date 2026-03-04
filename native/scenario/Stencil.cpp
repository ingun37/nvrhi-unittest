//
// Created by Ingun Jon on 3/4/26.
//

#include "Stencil.h"
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
};

nvrhi::GraphicsPipelineHandle create_pipeline(const Context& context, const Payload& payload) {
    nvrhi::GraphicsPipelineDesc psoDesc;
    psoDesc.VS = payload.vertex;
    psoDesc.PS = payload.pixel;
    psoDesc.primType = nvrhi::PrimitiveType::TriangleList;
    psoDesc.renderState.depthStencilState.depthTestEnable = false;
    psoDesc.renderState.depthStencilState.stencilEnable = true;
    psoDesc.renderState.depthStencilState.setFrontFaceStencil(nvrhi::DepthStencilState::StencilOpDesc{
        .passOp = nvrhi::StencilOp::Invert,
    });
    return context.nvrhiDevice->createGraphicsPipeline(psoDesc, payload.framebuffer);
}

struct RunDrawCommand : public Step {
    Payload payload;

    RunDrawCommand() = delete;

public:
    explicit RunDrawCommand(
        const Context& ctx,
        Payload&& payload
        )
        : Step(ctx, "RunDrawCommand", "", ""),
          payload(std::move(payload)) {
    }

    StepFuture run(std::string input) override {
        auto commandList = context.nvrhiDevice->createCommandList();
        commandList->open();

        {
            const nvrhi::FramebufferAttachment& att = payload.framebuffer->getDesc().colorAttachments[0];
            commandList->clearTextureFloat(att.texture, att.subresources, nvrhi::Color(0.1f, 0.3f, 0.6f, 1.0f));
        }

        {
            const nvrhi::FramebufferAttachment& att = payload.framebuffer->getDesc().depthAttachment;
            commandList->clearDepthStencilTexture(att.texture, att.subresources, true, 1.0f, true, 0);
        }

        nvrhi::GraphicsState state;
        auto pipeline = create_pipeline(context, payload);
        state.pipeline = pipeline;
        state.framebuffer = payload.framebuffer;
        state.viewport.addViewportAndScissorRect(payload.framebuffer->getFramebufferInfo().getViewport());
        commandList->setGraphicsState(state);
        nvrhi::DrawArguments args{};
        std::cout << "Drawing..." << std::endl;
        args.vertexCount = 3;
        commandList->draw(args);

        commandList->close();
        context.nvrhiDevice->executeCommandList(commandList);

        return create_null_step();
    }
};
}

StepFuture Stencil::run(std::string) {
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
    depthTextureDesc.setFormat(nvrhi::Format::D32S8);
    depthTextureDesc.setIsRenderTarget(true);
    auto depthTexture = context.nvrhiDevice->createTexture(depthTextureDesc);

    nvrhi::FramebufferAttachment depthAttachment{};
    depthAttachment.setTexture(depthTexture);

    nvrhi::FramebufferDesc framebufferDesc{};
    framebufferDesc.addColorAttachment(colorAttachment);
    framebufferDesc.setDepthAttachment(depthAttachment);
    auto framebuffer = context.nvrhiDevice->createFramebuffer(framebufferDesc);

    return create_step_immediately<RunDrawCommand>(
        context,
        Payload{
            std::move(colorTexture),
            std::move(depthTexture),
            std::move(vertex),
            std::move(pixel),
            std::move(framebuffer)
        });
}