//
// Created by Ingun Jon on 3/4/26.
//

#include "Stencil.h"
#include <iostream>
#include <fstream>
#include "backend.h"
#include <list>

namespace {
using float4 = std::array<float, 4>;
using float4x4 = std::array<float4, 4>;


constexpr float4x4 identity = {
    {{1, 0, 0, 0},
     {0, 1, 0, 0},
     {0, 0, 1, 0},
     {0, 0, 0, 1}}
};

float4x4 transform(float x, float z) {
    return {
        {{1, 0, 0, 0},
         {0, 1, 0, 0},
         {0, 0, 1, 0},
         {x, 0, z, 1}
        }
    };
}

struct Uniform {
    float4 color = {1, 0, 0, 1};
    float4x4 transform = identity;
};

struct Payload {
    nvrhi::TextureHandle colorTexture;
    nvrhi::TextureHandle depthTexture;
    nvrhi::ShaderHandle vertex;
    nvrhi::ShaderHandle pixel;
    nvrhi::FramebufferHandle framebuffer;
    nvrhi::BufferHandle constant_buffer;
    nvrhi::CommandListHandle command_list;
};

nvrhi::BindingLayoutHandle create_graphics_binding_layout(const nvrhi::DeviceHandle& device) {
    nvrhi::BindingLayoutItem bli{};
    bli.setSlot(0);
    bli.setType(nvrhi::ResourceType::ConstantBuffer);
    bli.setVisibility(nvrhi::ShaderType::AllGraphics);
    bli.setAccess(nvrhi::ResourceAccess::Read);
    bli.setSize(1);
    nvrhi::BindingLayoutDesc bld{};
    bld.setVisibility(nvrhi::ShaderType::AllGraphics);
    bld.addItem(bli);
    bld.setBindingOffsets(nvrhi::VulkanBindingOffsets{}.setConstantBufferOffset(0));
    return device->createBindingLayout(bld);
}

nvrhi::InputLayoutHandle create_graphics_input_layout(const nvrhi::DeviceHandle& device,
                                                      const Payload& payload
    ) {
    return device->createInputLayout(nullptr, 0, payload.vertex);
}

nvrhi::GraphicsPipelineHandle create_graphics_pipeline(const nvrhi::DeviceHandle& device,
                                                       const Payload& payload,
                                                       const nvrhi::BindingLayoutHandle& binding_layout
    ) {
    nvrhi::GraphicsPipelineDesc gpd;
    gpd.VS = payload.vertex;
    gpd.PS = payload.pixel;
    gpd.primType = nvrhi::PrimitiveType::TriangleList;
    gpd.renderState.depthStencilState.depthTestEnable = true;
    gpd.renderState.depthStencilState.depthFunc = nvrhi::ComparisonFunc::LessOrEqual;
    gpd.renderState.depthStencilState.depthWriteEnable = true;
    gpd.renderState.depthStencilState.stencilEnable = true;
    gpd.renderState.depthStencilState.setFrontFaceStencil(nvrhi::DepthStencilState::StencilOpDesc{
        .passOp = nvrhi::StencilOp::Invert,
    });
    gpd.addBindingLayout(binding_layout);
    auto input_layout = create_graphics_input_layout(device, payload);
    gpd.setInputLayout(input_layout);
    return device->createGraphicsPipeline(gpd, payload.framebuffer);
}

nvrhi::GraphicsState create_graphics_state(const Context& context,
                                           const Payload& payload,
                                           std::list<nvrhi::RefCountPtr<nvrhi::IResource> >& hold
    ) {
    auto binding_layout = create_graphics_binding_layout(context.nvrhiDevice);
    hold.emplace_back(binding_layout);
    auto pipeline = create_graphics_pipeline(context.nvrhiDevice, payload, binding_layout);
    hold.emplace_back(pipeline);
    auto bsi = nvrhi::BindingSetItem::ConstantBuffer(0, payload.constant_buffer);
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

struct RunDrawCommand : public Step {
    Payload payload;

    RunDrawCommand() = delete;

    std::list<nvrhi::RefCountPtr<nvrhi::IResource> > hold;

    explicit RunDrawCommand(
        const Context& ctx,
        Payload&& payload
        )
        : Step(ctx, "RunDrawCommand", "", ""),
          payload(std::move(payload)) {
    }

    StepFuture run(std::string input) override {
        payload.command_list->open();
        payload.command_list->beginTrackingBufferState(payload.constant_buffer, nvrhi::ResourceStates::ConstantBuffer);
        auto state = create_graphics_state(context, payload, hold);
        state.framebuffer = payload.framebuffer;
        Uniform uniform;
        uniform.transform = transform(0.0, 0.0);
        payload.command_list->writeBuffer(payload.constant_buffer, &uniform, sizeof(uniform));

        payload.command_list->setGraphicsState(state);
        nvrhi::DrawArguments args{};
        std::cout << "Drawing..." << std::endl;
        args.vertexCount = 3;
        payload.command_list->draw(args);
        payload.command_list->close();
        context.nvrhiDevice->executeCommandList(payload.command_list);

        return create_null_step();
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

        auto command_list = payload.command_list;
        command_list->open();
        command_list->clearTextureFloat(colorA.texture, colorA.subresources, {0.1f, 0.3f, 0.6f, 1.0f});
        command_list->clearDepthStencilTexture(depthA.texture, depthA.subresources, true, 1.0f, true, 0);
        command_list->close();
        context.nvrhiDevice->executeCommandList(command_list);

        return create_step_immediately<RunDrawCommand>(context, std::move(payload));
    }
};
}

StepFuture Stencil::run(std::string) {
    // Read data into vector
    auto file_data = read_shader("movable-triangle");

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

    nvrhi::BufferDesc bd{};
    bd.setIsConstantBuffer(true);
    bd.setByteSize(sizeof(Uniform));
    auto constant_buffer = context.nvrhiDevice->createBuffer(bd);

    auto command_list = context.nvrhiDevice->createCommandList();

    return create_step_immediately<Clear>(
        context,
        Payload{
            std::move(colorTexture),
            std::move(depthTexture),
            std::move(vertex),
            std::move(pixel),
            std::move(framebuffer),
            std::move(constant_buffer),
            std::move(command_list)
        });
}