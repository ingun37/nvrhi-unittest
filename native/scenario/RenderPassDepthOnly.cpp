//
// Created by Ingun Jon on 1/21/26.
//

#include "RenderPassDepthOnly.h"


StepFuture RunDepthDrawCommand::run(std::string) {
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

    return create_null_step();
}

StepFuture RenderPassDepthOnly::run(std::string) {
    nvrhi::ShaderDesc vertexDesc{
        .entryName = "vs"
    };
    const std::string vertexCode = R"(
        const g_positions = array<vec3f, 3>(
            vec3f(-0.5, -0.5, 0.1),
            vec3f(0.0, 0.5, 0.2),
            vec3f(0.5, -0.5, 0.3)
        );
        @vertex fn vs(@builtin(vertex_index) vertex_id: u32) -> @builtin(position) vec4f {
            return vec4f(g_positions[vertex_id], 1.0);
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
    psoDesc.renderState.depthStencilState.depthWriteEnable = true;

    psoDesc.renderState.blendState.targets[0].blendEnable = true;
    psoDesc.renderState.blendState.targets[0].srcBlend = nvrhi::BlendFactor::SrcAlpha;
    psoDesc.renderState.blendState.targets[0].destBlend = nvrhi::BlendFactor::OneMinusSrcAlpha;
    psoDesc.renderState.blendState.targets[0].blendOp = nvrhi::BlendOp::Add;
    psoDesc.renderState.blendState.targets[0].srcBlendAlpha = nvrhi::BlendFactor::One;
    psoDesc.renderState.blendState.targets[0].destBlendAlpha = nvrhi::BlendFactor::OneMinusSrcAlpha;
    psoDesc.renderState.blendState.targets[0].blendOpAlpha = nvrhi::BlendOp::Add;

    nvrhi::TextureDesc depthTextureDesc{};
    depthTextureDesc.setHeight(256);
    depthTextureDesc.setWidth(256);
    depthTextureDesc.setFormat(nvrhi::Format::D32);
    depthTextureDesc.setIsRenderTarget(true);
    auto depthTexture = context.nvrhiDevice->createTexture(depthTextureDesc);

    nvrhi::FramebufferAttachment depthAttachment{};
    depthAttachment.setTexture(depthTexture);
    nvrhi::FramebufferDesc framebufferDesc{};
    framebufferDesc.setDepthAttachment(depthAttachment);
    auto framebuffer = context.nvrhiDevice->createFramebuffer(framebufferDesc);

    auto pipeline = context.nvrhiDevice->createGraphicsPipeline(psoDesc, framebuffer);

    return create_step_immediately<RunDepthDrawCommand>(
        context,
        std::move(depthTexture),
        std::move(vertex),
        std::move(pixel),
        std::move(framebuffer),
        std::move(pipeline));
}