//
// Created by Ingun Jon on 2/28/26.
//

#include "DynamicRender.h"
#include <iostream>
#include <fstream>
#include <list>
#include <array>
#include "backend.h"

namespace {
using float4 = std::array<float, 4>;
using float4x4 = std::array<float4, 4>;


constexpr float4x4 identity = {
    {{1, 0, 0, 0},
     {0, 1, 0, 0},
     {0, 0, 1, 0},
     {0, 0, 0, 1}}
};

struct Uniform {
    float4 color = {1, 0, 0, 1};
    float4x4 transform = identity;
};

constexpr Uniform uniform_example;

struct Payload {
    nvrhi::BufferHandle vertex_buffer;
    nvrhi::ShaderHandle compute;
    nvrhi::CommandListHandle commandList;
    nvrhi::TextureHandle colorTexture;
    nvrhi::TextureHandle depthTexture;
    nvrhi::ShaderHandle vertex;
    nvrhi::ShaderHandle pixel;
    nvrhi::FramebufferHandle framebuffer;
    nvrhi::BufferHandle constantBuffer;
};

using vertex_t = std::array<float, 4>;
constexpr uint32_t horizontal_count = 2;
constexpr uint32_t vertical_count = 64;
constexpr uint32_t vertex_count = horizontal_count * vertical_count;


nvrhi::BindingLayoutHandle make_graphics_binding_layout(const nvrhi::DeviceHandle& device) {
    nvrhi::BindingLayoutItem bli{};
    bli.setSlot(0);
    bli.setType(nvrhi::ResourceType::ConstantBuffer);

    nvrhi::BindingLayoutItem bli2{};
    bli2.setSlot(1);
    bli2.setType(nvrhi::ResourceType::TypedBuffer_SRV);

    nvrhi::BindingLayoutDesc bld{};
    bld.setVisibility(nvrhi::ShaderType::AllGraphics);
    bld.addItem(bli);
    bld.addItem(bli2);

    return device->createBindingLayout(bld);
}

nvrhi::GraphicsPipelineHandle make_graphics_pipeline(const nvrhi::DeviceHandle& device,
                                                     const Payload& payload,
                                                     const nvrhi::BindingLayoutHandle& binding_layout,
                                                     const float depthBiasSlopeScale
    ) {
    nvrhi::GraphicsPipelineDesc gpd;
    gpd.VS = payload.vertex;
    gpd.PS = payload.pixel;
    gpd.primType = nvrhi::PrimitiveType::TriangleStrip;
    gpd.renderState.depthStencilState.depthTestEnable = true;
    gpd.renderState.depthStencilState.depthFunc = nvrhi::ComparisonFunc::LessOrEqual;
    gpd.renderState.depthStencilState.depthWriteEnable = true;
    gpd.renderState.blendState.targets[0].blendEnable = true;
    gpd.renderState.blendState.targets[0].srcBlend = nvrhi::BlendFactor::SrcAlpha;
    gpd.renderState.blendState.targets[0].destBlend = nvrhi::BlendFactor::OneMinusSrcAlpha;
    gpd.renderState.blendState.targets[0].blendOp = nvrhi::BlendOp::Add;
    gpd.renderState.blendState.targets[0].srcBlendAlpha = nvrhi::BlendFactor::One;
    gpd.renderState.blendState.targets[0].destBlendAlpha = nvrhi::BlendFactor::OneMinusSrcAlpha;
    gpd.renderState.blendState.targets[0].blendOpAlpha = nvrhi::BlendOp::Add;
    gpd.renderState.rasterState.slopeScaledDepthBias = depthBiasSlopeScale;

    gpd.addBindingLayout(binding_layout);
    return device->createGraphicsPipeline(gpd, payload.framebuffer);
}

nvrhi::GraphicsState make_graphics_state(const Context& context,
                                         const Payload& payload,
                                         std::list<nvrhi::RefCountPtr<nvrhi::IResource> >& hold,
                                         const float depthBiasSlopeScale
    ) {
    auto binding_layout = make_graphics_binding_layout(context.nvrhiDevice);
    hold.emplace_back(binding_layout);
    auto pipeline = make_graphics_pipeline(context.nvrhiDevice, payload, binding_layout, depthBiasSlopeScale);
    hold.emplace_back(pipeline);
    auto bsi = nvrhi::BindingSetItem::ConstantBuffer(0, payload.constantBuffer);
    auto bsi2 = nvrhi::BindingSetItem::TypedBuffer_SRV(1, payload.vertex_buffer, nvrhi::Format::RGBA32_FLOAT);

    nvrhi::BindingSetDesc bsd{};
    bsd.addItem(bsi);
    bsd.addItem(bsi2);
    auto bs = context.nvrhiDevice->createBindingSet(bsd, binding_layout);
    hold.emplace_back(bs);
    nvrhi::GraphicsState state;
    state.setPipeline(pipeline);
    state.setFramebuffer(payload.framebuffer);
    state.viewport.addViewportAndScissorRect(payload.framebuffer->getFramebufferInfo().getViewport());
    state.addBindingSet(bs);
    return std::move(state);
}

nvrhi::BindingLayoutHandle make_compute_binding_layout(const nvrhi::DeviceHandle& device) {
    nvrhi::BindingLayoutItem bli{};
    bli.setSlot(0);
    bli.setType(nvrhi::ResourceType::TypedBuffer_UAV);
    bli.setFormat(nvrhi::Format::RGBA32_FLOAT);
    bli.setSize(sizeof(vertex_t) * vertex_count);
    nvrhi::BindingLayoutDesc bld{};
    bld.setVisibility(nvrhi::ShaderType::Compute);
    bld.addItem(bli);
    return device->createBindingLayout(bld);
}

nvrhi::ComputePipelineHandle make_compute_pipeline(const nvrhi::DeviceHandle& device,
                                                   const Payload& payload,
                                                   const nvrhi::BindingLayoutHandle& binding_layout
    ) {
    nvrhi::ComputePipelineDesc gpd;
    gpd.setComputeShader(payload.compute);
    gpd.addBindingLayout(binding_layout);
    return device->createComputePipeline(gpd);
}

nvrhi::ComputeState make_compute_state(const Context& context,
                                       const Payload& payload,
                                       std::list<nvrhi::RefCountPtr<nvrhi::IResource> >& hold
    ) {
    auto binding_layout = make_compute_binding_layout(context.nvrhiDevice);
    hold.emplace_back(binding_layout);
    auto pipeline = make_compute_pipeline(context.nvrhiDevice, payload, binding_layout);
    hold.emplace_back(pipeline);
    auto bsi = nvrhi::BindingSetItem::TypedBuffer_UAV(0, payload.vertex_buffer);
    nvrhi::BindingSetDesc bsd{};
    bsd.addItem(bsi);
    auto bs = context.nvrhiDevice->createBindingSet(bsd, binding_layout);
    hold.emplace_back(bs);
    nvrhi::ComputeState state;
    state.setPipeline(pipeline);
    state.addBindingSet(bs);
    return std::move(state);
}

struct Draw : Step {
    Payload payload;

    Draw() = delete;

    Draw(const Context& ctx, Payload&& payload)
        : Step(ctx, "Draw", "", ""),
          payload(std::move(payload)) {
    }

    StepFuture run(std::string) override {
        std::list<nvrhi::RefCountPtr<nvrhi::IResource> > hold;
        auto state = make_graphics_state(context, payload, hold, 0);
        payload.commandList->open();
        Uniform uniform{};
        uniform.color = float4{0, 0, 1, 1};
        uniform.transform[3][0] = 0.0;
        payload.commandList->writeBuffer(payload.constantBuffer, &uniform, sizeof(uniform));
        payload.commandList->setGraphicsState(state);
        payload.commandList->draw({.vertexCount = 10});
        payload.commandList->close();

        context.nvrhiDevice->executeCommandList(payload.commandList);
        return create_null_step();
    }
};

struct Compute : public Step {
    Payload payload;

    Compute() = delete;

    Compute(const Context& ctx,
            Payload&& payload
        )
        : Step(ctx, "Compute", "", ""),
          payload(std::move(payload)) {
    }

    StepFuture run(std::string) override {
        std::list<nvrhi::RefCountPtr<nvrhi::IResource> > hold;
        auto state = make_compute_state(context, payload, hold);
        payload.commandList->open();
        payload.commandList->setComputeState(state);
        payload.commandList->dispatch(1, 1, 1);
        payload.commandList->close();

        context.nvrhiDevice->executeCommandList(payload.commandList);
        return create_step_immediately<Draw>(context, std::move(payload));
    }
};
}

StepFuture DynamicRender::run(std::string input) {
    auto csBin = read_shader("generate-geometry");
    nvrhi::ShaderDesc computeDesc{};
    computeDesc.setEntryName("cs");
    computeDesc.setShaderType(nvrhi::ShaderType::Compute);
    auto compute = context.nvrhiDevice->createShader(computeDesc, csBin.data(), csBin.size());

    auto vsPsBin = read_shader("draw-buffer");

    nvrhi::ShaderDesc vertexDesc{};
    vertexDesc.setEntryName("vs");
    vertexDesc.setShaderType(nvrhi::ShaderType::Vertex);
    nvrhi::ShaderDesc pixelDesc{};
    pixelDesc.setEntryName("ps");
    pixelDesc.setShaderType(nvrhi::ShaderType::Pixel);
    auto vertex = context.nvrhiDevice->createShader(vertexDesc, vsPsBin.data(), vsPsBin.size());
    auto pixel = context.nvrhiDevice->createShader(pixelDesc, vsPsBin.data(), vsPsBin.size());

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
    depthTextureDesc.setFormat(nvrhi::Format::D32);
    depthTextureDesc.setIsRenderTarget(true);
    auto depthTexture = context.nvrhiDevice->createTexture(depthTextureDesc);

    nvrhi::FramebufferAttachment depthAttachment{};
    depthAttachment.setTexture(depthTexture);

    nvrhi::FramebufferDesc framebufferDesc{};
    framebufferDesc.addColorAttachment(colorAttachment);
    framebufferDesc.setDepthAttachment(depthAttachment);
    auto framebuffer = context.nvrhiDevice->createFramebuffer(framebufferDesc);

    nvrhi::BufferDesc vertex_buffer_desc{};
    vertex_buffer_desc.setCanHaveUAVs(true);
    vertex_buffer_desc.setByteSize(sizeof(vertex_t) * vertex_count);
    auto vertex_buffer = context.nvrhiDevice->createBuffer(vertex_buffer_desc);
    auto command = context.nvrhiDevice->createCommandList();

    nvrhi::BufferDesc bd{};
    bd.setIsConstantBuffer(true);
    bd.setByteSize(sizeof(uniform_example));

    auto constantBuffer = context.nvrhiDevice->createBuffer(bd);

    return create_step_immediately<Compute>(context,
                                            Payload{
                                                std::move(vertex_buffer),
                                                std::move(compute),
                                                std::move(command),
                                                std::move(colorTexture),
                                                std::move(depthTexture),
                                                std::move(vertex),
                                                std::move(pixel),
                                                std::move(framebuffer),
                                                std::move(constantBuffer)
                                            });
}