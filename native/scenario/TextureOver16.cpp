//
// Created by Ingun Jon on 2/9/26.
//

#include "TextureOver16.h"
#include <iostream>
#include <fstream>
#include "backend.h"
#include <ranges>

static constexpr int texture_number = 16;

StepFuture TextureOver16Draw::run(std::string) {
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

    int cnt = 0;
    auto bliArr = textures | std::views::transform([&cnt](auto texture) {
        return nvrhi::BindingLayoutItem::Texture_SRV(cnt++);
    });
    nvrhi::BindingLayoutDesc bld{};
    bld.setVisibility(nvrhi::ShaderType::AllGraphics);
    for (const auto& bli : bliArr) {
        bld.addItem(bli);
    }
    auto bl = context.nvrhiDevice->createBindingLayout(bld);
    gpd.addBindingLayout(bl);
    auto pipeline = context.nvrhiDevice->createGraphicsPipeline(gpd, framebuffer);
    cnt = 0;
    auto bsiArr = textures | std::views::transform([&cnt](auto texture) {
        return nvrhi::BindingSetItem::Texture_SRV(cnt++, texture);
    });

    nvrhi::BindingSetDesc bsd{};
    for (const auto& bsi : bsiArr) {
        bsd.addItem(bsi);
    }
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

    return create_null_step();
}

StepFuture TextureOver16::run(std::string) {
    std::string shader_dir = SCENARIO_SHADERS_OUTPUT_DIR;
    std::string path = shader_dir + "/texture" + extension();
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

    auto textures = std::views::iota(0, texture_number) | std::views::transform([this](auto idx) {
        nvrhi::TextureDesc desc{};
        desc.setWidth(8);
        desc.setHeight(8);
        desc.setFormat(nvrhi::Format::RGBA8_UNORM);
        return context.nvrhiDevice->createTexture(desc);
    }) | std::ranges::to<std::vector>();
    auto commandList = context.nvrhiDevice->createCommandList();

    return create_step_immediately<TextureOver16Draw>(context,
                                                      std::move(colorTexture),
                                                      std::move(vertex),
                                                      std::move(pixel),
                                                      std::move(framebuffer),
                                                      std::move(commandList),
                                                      std::move(textures)
        );
}