//
// Created by Ingun Jon on 2/26/26.
//

#include "MovableTriangle.h"
#include <iostream>
#include <fstream>
#include "backend.h"

namespace {
using float4 = std::array<float, 4>;
using float4x4 = std::array<float4, 4>;
static float4x4 identity = {
    {{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}
};

struct Payload {
    nvrhi::TextureHandle colorTexture;
    nvrhi::ShaderHandle vertex;
    nvrhi::ShaderHandle pixel;
    nvrhi::FramebufferHandle framebuffer;
    nvrhi::BufferHandle constantBuffer;
    nvrhi::CommandListHandle commandList;
};

struct ConstantBufferDraw : public Step {
    Payload payload;

    ConstantBufferDraw() = delete;

    explicit ConstantBufferDraw(const Context& ctx,
                                Payload&& payload
        )
        : Step(ctx, "RunDrawCommand", "", ""),
          payload(std::move(payload)) {
    }

    StepFuture run(std::string) override {
        nvrhi::GraphicsPipelineDesc gpd;
        gpd.VS = payload.vertex;
        gpd.PS = payload.pixel;
        gpd.primType = nvrhi::PrimitiveType::TriangleList;
        gpd.renderState.depthStencilState.depthTestEnable = false;
        gpd.renderState.blendState.targets[0].blendEnable = true;
        gpd.renderState.blendState.targets[0].srcBlend = nvrhi::BlendFactor::SrcAlpha;
        gpd.renderState.blendState.targets[0].destBlend = nvrhi::BlendFactor::OneMinusSrcAlpha;
        gpd.renderState.blendState.targets[0].blendOp = nvrhi::BlendOp::Add;
        gpd.renderState.blendState.targets[0].srcBlendAlpha = nvrhi::BlendFactor::One;
        gpd.renderState.blendState.targets[0].destBlendAlpha = nvrhi::BlendFactor::OneMinusSrcAlpha;
        gpd.renderState.blendState.targets[0].blendOpAlpha = nvrhi::BlendOp::Add;
        nvrhi::BindingLayoutItem bli{};
        bli.setSlot(0);
        bli.setType(nvrhi::ResourceType::ConstantBuffer);
        nvrhi::BindingLayoutDesc bld{};
        bld.setVisibility(nvrhi::ShaderType::AllGraphics);
        bld.addItem(bli);
        auto bl = context.nvrhiDevice->createBindingLayout(bld);
        gpd.addBindingLayout(bl);
        auto pipeline = context.nvrhiDevice->createGraphicsPipeline(gpd, payload.framebuffer);
        auto bsi = nvrhi::BindingSetItem::ConstantBuffer(0, payload.constantBuffer);
        nvrhi::BindingSetDesc bsd{};
        bsd.addItem(bsi);
        auto bs = context.nvrhiDevice->createBindingSet(bsd, bl);
        nvrhi::GraphicsState state;
        state.setPipeline(pipeline);
        state.setFramebuffer(payload.framebuffer);
        state.viewport.addViewportAndScissorRect(payload.framebuffer->getFramebufferInfo().getViewport());
        state.addBindingSet(bs);

        payload.commandList->open();
        payload.commandList->setGraphicsState(state);
        payload.commandList->draw({.vertexCount = 3});
        payload.commandList->close();

        context.nvrhiDevice->executeCommandList(payload.commandList);

        return create_null_step();
    }
};
}

StepFuture MovableTriangle::run(std::string) {
    std::string shader_dir = SCENARIO_SHADERS_OUTPUT_DIR;
    std::string path = shader_dir + "/movable-triangle" + extension();
    std::ifstream file(path, std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open shader file at " + path);
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

    nvrhi::BufferDesc bd{};
    bd.setIsConstantBuffer(true);
    bd.setByteSize(sizeof(identity));

    auto constantBuffer = context.nvrhiDevice->createBuffer(bd);

    auto commandList = context.nvrhiDevice->createCommandList();
    commandList->open();
    commandList->writeBuffer(constantBuffer, &identity, sizeof(identity));
    commandList->close();
    context.nvrhiDevice->executeCommandList(commandList);

    return create_step_immediately<ConstantBufferDraw>(context,
                                                       Payload{
                                                           std::move(colorTexture),
                                                           std::move(vertex),
                                                           std::move(pixel),
                                                           std::move(framebuffer),
                                                           std::move(constantBuffer),
                                                           std::move(commandList)
                                                       });
}