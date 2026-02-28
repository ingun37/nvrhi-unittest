//
// Created by Ingun Jon on 2/26/26.
//

#include "DepthBias.h"
#include <iostream>
#include <fstream>
#include <list>
#include <sstream>

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

float4x4 translate_x(float x) {
    return {
        {{1, 0, 0, 0},
         {0, 1, 0, 0},
         {0, 0, 1, 0},
         {x, 0, 0, 1}
        }
    };
}

struct Uniform {
    float4 color = {1, 0, 0, 1};
    float4x4 transform = identity;
};

constexpr Uniform uniform_example;

struct Param {
    Uniform uniform;
};

struct Payload {
    nvrhi::TextureHandle colorTexture;
    nvrhi::TextureHandle depthTexture;
    nvrhi::ShaderHandle vertex;
    nvrhi::ShaderHandle pixel;
    nvrhi::FramebufferHandle framebuffer;
    nvrhi::BufferHandle constantBuffer;
    nvrhi::CommandListHandle commandList;
};

nvrhi::BindingLayoutHandle make_graphics_binding_layout(const nvrhi::DeviceHandle& device) {
    nvrhi::BindingLayoutItem bli{};
    bli.setSlot(0);
    bli.setType(nvrhi::ResourceType::ConstantBuffer);
    nvrhi::BindingLayoutDesc bld{};
    bld.setVisibility(nvrhi::ShaderType::AllGraphics);
    bld.addItem(bli);
    return device->createBindingLayout(bld);
}

nvrhi::GraphicsPipelineHandle make_graphics_pipeline(const nvrhi::DeviceHandle& device,
                                                     const Payload& payload,
                                                     const nvrhi::BindingLayoutHandle& binding_layout,
                                                     const float bias,
                                                     const float clamp
    ) {
    nvrhi::GraphicsPipelineDesc gpd;
    gpd.VS = payload.vertex;
    gpd.PS = payload.pixel;
    gpd.primType = nvrhi::PrimitiveType::TriangleList;
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
    gpd.renderState.rasterState.depthBias = bias;
    gpd.renderState.rasterState.depthBiasClamp = clamp;

    gpd.addBindingLayout(binding_layout);
    return device->createGraphicsPipeline(gpd, payload.framebuffer);
}

nvrhi::GraphicsState make_graphics_state(const Context& context,
                                         const Payload& payload,
                                         std::list<nvrhi::RefCountPtr<nvrhi::IResource> >& hold,
                                         const float bias,
                                         const float clamp
    ) {
    auto binding_layout = make_graphics_binding_layout(context.nvrhiDevice);
    hold.emplace_back(binding_layout);
    auto pipeline = make_graphics_pipeline(context.nvrhiDevice, payload, binding_layout, bias, clamp);
    hold.emplace_back(pipeline);
    auto bsi = nvrhi::BindingSetItem::ConstantBuffer(0, payload.constantBuffer);
    nvrhi::BindingSetDesc bsd{};
    bsd.addItem(bsi);
    auto bs = context.nvrhiDevice->createBindingSet(bsd, binding_layout);
    hold.emplace_back(bs);
    nvrhi::GraphicsState state;
    state.setPipeline(pipeline);
    state.setFramebuffer(payload.framebuffer);
    state.viewport.addViewportAndScissorRect(payload.framebuffer->getFramebufferInfo().getViewport());
    state.addBindingSet(bs);
    return std::move(state);
}

struct Draw : Step {
    Payload payload;

    Draw() = delete;

    std::list<Param> params;

    Draw(const Context& ctx, Payload&& payload, std::list<Param>&& params)
        : Step(ctx, "Draw", "bias, clamp", "0.0 0.0"),
          payload(std::move(payload)),
          params(std::move(params)) {
    }

    StepFuture run(std::string input) override {
        auto param = params.front();
        float bias, clamp;
        try {
            std::stringstream ss(input);
            ss >> bias >> clamp;
        } catch (...) {
            throw std::runtime_error("Invalid input");
        }
        auto uniform = param.uniform;
        std::cout << "Using bias: " << bias << " clamp: " << clamp << " color: (" << uniform.color[0] << ", " << uniform
            .
            color[1] << ", " << uniform.color[2] << ")" << std::endl;
        std::list<nvrhi::RefCountPtr<nvrhi::IResource> > hold;
        params.pop_front();

        auto state = make_graphics_state(context, payload, hold, bias, clamp);
        payload.commandList->open();
        payload.commandList->writeBuffer(payload.constantBuffer, &uniform, sizeof(uniform));
        payload.commandList->setGraphicsState(state);
        payload.commandList->draw({.vertexCount = 3});
        payload.commandList->close();

        context.nvrhiDevice->executeCommandList(payload.commandList);
        if (params.empty())
            return create_null_step();
        else {
            return create_step_immediately<Draw>(context, std::move(payload), std::move(params));
        }
    }
};

struct Clear : public Step {
    Payload payload;

    Clear(
        const Context& ctx,
        Payload&& payload)
        : Step(ctx, "Clear", "", ""),
          payload(std::move(payload)) {
    }

    StepFuture run(std::string input) override {
        const nvrhi::FramebufferAttachment& colorA = payload.framebuffer->getDesc().colorAttachments[0];
        const nvrhi::FramebufferAttachment& depthA = payload.framebuffer->getDesc().depthAttachment;

        auto commandList = payload.commandList;
        commandList->open();
        commandList->clearTextureFloat(colorA.texture, colorA.subresources, {0, 0, 0, 0});
        commandList->clearDepthStencilTexture(depthA.texture, depthA.subresources, true, 1.0f, false, 0);
        commandList->close();
        context.nvrhiDevice->executeCommandList(commandList);

        std::list<Param> uniforms = {
            Param{Uniform{{1, 0, 0, 1}, translate_x(-0.2)}},
            Param{Uniform{{0, 1, 0, 1}, translate_x(0.0)}},
            Param{Uniform{{0, 0, 1, 1}, translate_x(0.2)}},
        };

        return create_step_immediately<Draw>(context, std::move(payload), std::move(uniforms));
    }
};
}

StepFuture DepthBias::run(std::string) {
    auto fileData = read_shader("movable-triangle");

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

    nvrhi::BufferDesc bd{};
    bd.setIsConstantBuffer(true);
    bd.setByteSize(sizeof(uniform_example));

    auto constantBuffer = context.nvrhiDevice->createBuffer(bd);

    auto commandList = context.nvrhiDevice->createCommandList();
    commandList->open();
    commandList->writeBuffer(constantBuffer, &uniform_example, sizeof(uniform_example));
    commandList->close();
    context.nvrhiDevice->executeCommandList(commandList);

    return create_step_immediately<Clear>(context,
                                          Payload{
                                              std::move(colorTexture),
                                              std::move(depthTexture),
                                              std::move(vertex),
                                              std::move(pixel),
                                              std::move(framebuffer),
                                              std::move(constantBuffer),
                                              std::move(commandList),
                                          });
}