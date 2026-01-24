//
// Created by Ingun Jon on 1/21/26.
//

#include "RenderPassColorClearDraw.h"

#include "Termination.h"
#include <iostream>
#include <fstream>


std::string padToMultipleOfFour(std::string input) {
    size_t remainder = input.length() % 4;
    if (remainder != 0) {
        size_t paddingNeeded = 4 - remainder;
        input.append(paddingNeeded, ' ');
    }
    return input;
}

AppPtr RunDrawCommand::run(std::string input) {
    std::istringstream iss(input);
    std::string clearStr, drawStr;
    iss >> clearStr >> drawStr;

    bool shouldClear = (clearStr == "true");
    bool shouldDraw = (drawStr == "true");

    auto commandList = context.nvrhiDevice->createCommandList();
    commandList->open();

    nvrhi::GraphicsState state;
    state.pipeline = pipeline;
    state.framebuffer = framebuffer;
    state.viewport.addViewportAndScissorRect(framebuffer->getFramebufferInfo().getViewport());
    commandList->setGraphicsState(state);

    const nvrhi::FramebufferAttachment& att = framebuffer->getDesc().colorAttachments[0];
    if (shouldClear) {
        std::cout << "Clearing..." << std::endl;
        commandList->clearTextureFloat(att.texture, att.subresources, nvrhi::Color(0.1f, 0.3f, 0.6f, 1.0f));
    }

    nvrhi::DrawArguments args{};
    if (shouldDraw) {
        std::cout << "Drawing..." << std::endl;
        args.vertexCount = 3;
        commandList->draw(args);
    }

    commandList->close();
    context.nvrhiDevice->executeCommandList(commandList);

    return immediate_app(std::make_unique<RunDrawCommand>(
        context,
        std::move(texture),
        std::move(vertex),
        std::move(pixel),
        std::move(framebuffer),
        std::move(pipeline)));
}

std::string extension() {
#if defined(SCENARIO_VULKAN)
    return ".sprv";
#elif defined(SCENARIO_WGPU)
    return ".wgsl";
#else
    throw std::runtime_error("Unknown platform");
#endif
}

AppPtr RenderPassColorClearDraw::run(std::string) {
    std::string shader_dir = SCENARIO_SHADERS_OUTPUT_DIR;
    std::ifstream file(shader_dir + "/simple" + extension(), std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open shader file");
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
    nvrhi::FramebufferDesc framebufferDesc{};
    framebufferDesc.addColorAttachment(colorAttachment);
    auto framebuffer = context.nvrhiDevice->createFramebuffer(framebufferDesc);

    auto pipeline = context.nvrhiDevice->createGraphicsPipeline(psoDesc, framebuffer);

    return immediate_app(std::make_unique<RunDrawCommand>(
        context,
        std::move(colorTexture),
        std::move(vertex),
        std::move(pixel),
        std::move(framebuffer),
        std::move(pipeline)));
}