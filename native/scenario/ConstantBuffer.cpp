//
// Created by Ingun Jon on 2/3/26.
//

#include "ConstantBuffer.h"
#include <iostream>
#include <fstream>
#include "backend.h"
constexpr const char dummy32[32] = {};

AppPtr ConstantBufferDraw::run(std::string) {
    nvrhi::GraphicsPipelineDesc gpd;
    gpd.VS = vertex;
    gpd.PS = pixel;
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
    auto pipeline = context.nvrhiDevice->createGraphicsPipeline(gpd, framebuffer);
    auto bsi = nvrhi::BindingSetItem::ConstantBuffer(0, constantBuffer);
    nvrhi::BindingSetDesc bsd{};
    bsd.addItem(bsi);
    auto bs = context.nvrhiDevice->createBindingSet(bsd, bl);
    nvrhi::GraphicsState state;
    state.setPipeline(pipeline);
    state.setFramebuffer(framebuffer);
    state.viewport.addViewportAndScissorRect(framebuffer->getFramebufferInfo().getViewport());
    state.addBindingSet(bs);

    commandList->open();
    commandList->setGraphicsState(state);
    commandList->draw({.vertexCount = 3});
    commandList->close();
    context.nvrhiDevice->executeCommandList(commandList);

    return nullptr;
}

AppPtr ConstantBuffer::run(std::string) {
    std::string shader_dir = SCENARIO_SHADERS_OUTPUT_DIR;
    std::string path = shader_dir + "/constant-buffer" + extension();
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
    bd.setByteSize(sizeof(dummy32));

    auto constantBuffer = context.nvrhiDevice->createBuffer(bd);

    auto commandList = context.nvrhiDevice->createCommandList();
    commandList->open();
    commandList->writeBuffer(constantBuffer, dummy32, sizeof(dummy32));
    commandList->close();
    context.nvrhiDevice->executeCommandList(commandList);

    return create_app_immediately<ConstantBufferDraw>(context,
                                                      std::move(colorTexture),
                                                      std::move(vertex),
                                                      std::move(pixel),
                                                      std::move(framebuffer),
                                                      std::move(constantBuffer),
                                                      std::move(commandList));
}