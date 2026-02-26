//
// Created by Ingun Jon on 2/26/26.
//

#include "ConstantBlend.h"
#include "backend.h"
#include <iostream>
#include <fstream>

namespace {
struct Payload {
    nvrhi::TextureHandle colorTexture;
    nvrhi::ShaderHandle vertex;
    nvrhi::ShaderHandle pixel;
    nvrhi::FramebufferHandle framebuffer;
    nvrhi::GraphicsPipelineHandle pipeline;
};

struct RunDrawCommand : public Step {
    Payload payload;

    RunDrawCommand() = delete;

public:
    explicit RunDrawCommand(
        const Context& ctx,
        Payload&& payload
        )
        : Step(ctx, "RunDrawCommand", "", "true true true"),
          payload(payload) {
    }

    StepFuture run(std::string input) override {
        auto commandList = context.nvrhiDevice->createCommandList();
        commandList->open();

        const nvrhi::FramebufferAttachment& att = payload.framebuffer->getDesc().colorAttachments[0];
        std::cout << "Clearing Color..." << std::endl;
        commandList->clearTextureFloat(att.texture, att.subresources, nvrhi::Color(0.6f, 0.3f, 0.1f, 1.0f));

        nvrhi::GraphicsState state;

        state.pipeline = payload.pipeline;
        state.framebuffer = payload.framebuffer;
        state.viewport.addViewportAndScissorRect(payload.framebuffer->getFramebufferInfo().getViewport());
        state.blendConstantColor = nvrhi::Color(0.0f, 0.0f, 1.0f, 1.0f);

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

StepFuture ConstantBlend::run(std::string) {
    std::string shader_dir = SCENARIO_SHADERS_OUTPUT_DIR;
    std::ifstream file(shader_dir + "/almost-white-triangle" + extension(), std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error(
            "Failed to open shader file at " + shader_dir + "/almost-white-triangle" + extension());
    }
    // Determine file size
    file.seekg(0, std::ios::end);
    std::streampos fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // Read data into vector
    std::vector<char> fileData(fileSize);
    file.read(fileData.data(), fileSize);

    nvrhi::ShaderDesc vertexDesc{};
    vertexDesc.setEntryName("vs");
    vertexDesc.setShaderType(nvrhi::ShaderType::Vertex);
    nvrhi::ShaderDesc pixelDesc{};
    pixelDesc.setEntryName("ps");
    pixelDesc.setShaderType(nvrhi::ShaderType::Pixel);
    auto vertex = context.nvrhiDevice->createShader(vertexDesc, fileData.data(), fileData.size());
    auto pixel = context.nvrhiDevice->createShader(pixelDesc, fileData.data(), fileData.size());

    nvrhi::GraphicsPipelineDesc psoDesc;
    psoDesc.VS = vertex;
    psoDesc.PS = pixel;
    psoDesc.primType = nvrhi::PrimitiveType::TriangleList;

    psoDesc.renderState.blendState.targets[0].blendEnable = true;
    psoDesc.renderState.blendState.targets[0].srcBlend = nvrhi::BlendFactor::ConstantColor;
    psoDesc.renderState.blendState.targets[0].destBlend = nvrhi::BlendFactor::Zero;
    psoDesc.renderState.blendState.targets[0].blendOp = nvrhi::BlendOp::Add;
    psoDesc.renderState.blendState.targets[0].srcBlendAlpha = nvrhi::BlendFactor::ConstantColor;
    psoDesc.renderState.blendState.targets[0].destBlendAlpha = nvrhi::BlendFactor::Zero;
    psoDesc.renderState.blendState.targets[0].blendOpAlpha = nvrhi::BlendOp::Add;

    nvrhi::TextureDesc colorTextureDesc{};
    colorTextureDesc.setHeight(256);
    colorTextureDesc.setWidth(256);
    colorTextureDesc.setFormat(nvrhi::Format::RGBA8_UNORM);
    colorTextureDesc.setIsRenderTarget(true);
    auto colorTexture = context.nvrhiDevice->createTexture(colorTextureDesc);
    nvrhi::FramebufferAttachment colorAttachment{};
    colorAttachment.setTexture(colorTexture);

    nvrhi::FramebufferDesc framebufferDesc{};
    framebufferDesc.addColorAttachment(colorAttachment);
    auto framebuffer = context.nvrhiDevice->createFramebuffer(framebufferDesc);

    auto pipeline = context.nvrhiDevice->createGraphicsPipeline(psoDesc, framebuffer);

    return create_step_immediately<RunDrawCommand>(
        context,
        Payload(std::move(colorTexture),
                std::move(vertex),
                std::move(pixel),
                std::move(framebuffer),
                std::move(pipeline)));
}