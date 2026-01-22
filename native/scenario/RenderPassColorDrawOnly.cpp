//
// Created by Ingun Jon on 1/21/26.
//

#include "RenderPassColorDrawOnly.h"

#include "Termination.h"

AppPtr RunDrawCommand::run(std::string) {
    auto commandList = context.nvrhiDevice->createCommandList();
    commandList->open();

    nvrhi::GraphicsState state;
    state.pipeline = pipeline;
    state.framebuffer = framebuffer;
    state.viewport.addViewportAndScissorRect(framebuffer->getFramebufferInfo().getViewport());
    commandList->setGraphicsState(state);

    nvrhi::DrawArguments args;
    args.vertexCount = 3;
    commandList->draw(args);

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

AppPtr RenderPassColorDrawOnly::run(std::string) {
    nvrhi::ShaderDesc vertexDesc{
        .entryName = "vs"
    };
    const std::string vertexCode = R"(
        const g_positions = array<vec2f, 3>(
            vec2f(-0.5, -0.5),
            vec2f(0.0, 0.5),
            vec2f(0.5, -0.5)
        );
        @vertex fn vs(@builtin(vertex_index) vertex_id: u32) -> @builtin(position) vec4f {
            return vec4f(g_positions[vertex_id], 0.0, 1.0);
        }
    )";
    nvrhi::ShaderDesc pixelDesc{
        .entryName = "fs"
    };
    const std::string pixelCode = R"(
        @fragment fn fs(@builtin(position) FragCoord : vec4f) -> @location(0) vec4f {
            return vec4f(1, 0, 0, 1);
        }
    )";

    auto vertex = context.nvrhiDevice->createShader(vertexDesc, vertexCode.c_str(), vertexCode.size());
    auto pixel = context.nvrhiDevice->createShader(pixelDesc, pixelCode.c_str(), pixelCode.size());

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