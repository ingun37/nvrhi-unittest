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
                                                       const nvrhi::BindingLayoutHandle& binding_layout,
                                                       const bool stencil,
                                                       const bool use_dynamic_ref,
                                                       const bool back_face,
                                                       const uint8_t static_stencil_ref
    ) {
    nvrhi::GraphicsPipelineDesc gpd;
    gpd.VS = payload.vertex;
    gpd.PS = payload.pixel;
    gpd.primType = nvrhi::PrimitiveType::TriangleList;
    gpd.renderState.rasterState.cullMode = back_face ? nvrhi::RasterCullMode::Front : nvrhi::RasterCullMode::Back;
    gpd.renderState.rasterState.setFrontCounterClockwise(back_face);
    auto& ds = gpd.renderState.depthStencilState;
    ds.depthTestEnable = true;
    ds.depthFunc = nvrhi::ComparisonFunc::LessOrEqual;
    ds.depthWriteEnable = true;
    ds.stencilEnable = stencil;
    ds.setStencilRefValue(static_stencil_ref);
    ds.setFrontFaceStencil(nvrhi::DepthStencilState::StencilOpDesc{
        .failOp = nvrhi::StencilOp::Invert,
        .depthFailOp = nvrhi::StencilOp::DecrementAndWrap,
        .passOp = nvrhi::StencilOp::Replace,
        .stencilFunc = nvrhi::ComparisonFunc::Greater,
    });
    ds.setBackFaceStencil(nvrhi::DepthStencilState::StencilOpDesc{
        .failOp = nvrhi::StencilOp::Zero,
        .depthFailOp = nvrhi::StencilOp::Invert,
        .passOp = nvrhi::StencilOp::Replace,
        .stencilFunc = nvrhi::ComparisonFunc::LessOrEqual,
    });
    ds.setDynamicStencilRef(use_dynamic_ref);
    gpd.addBindingLayout(binding_layout);
    auto input_layout = create_graphics_input_layout(device, payload);
    gpd.setInputLayout(input_layout);
    return device->createGraphicsPipeline(gpd, payload.framebuffer);
}

nvrhi::BindingSetHandle create_graphics_binding_set(const Context& context,
                                                    const Payload& payload,
                                                    const nvrhi::BindingLayoutHandle& binding_layout
    ) {
    auto bsi = nvrhi::BindingSetItem::ConstantBuffer(0, payload.constant_buffer);
    nvrhi::BindingSetDesc bsd{};
    bsd.addItem(bsi);
    return context.nvrhiDevice->createBindingSet(bsd, binding_layout);
}

nvrhi::GraphicsState create_graphics_state(const Context& context,
                                           const Payload& payload,
                                           const nvrhi::GraphicsPipelineHandle& pipeline,
                                           const nvrhi::BindingSetHandle& binding_set
    ) {
    nvrhi::GraphicsState state;
    state.setPipeline(pipeline);
    state.setFramebuffer(payload.framebuffer);
    state.viewport.addViewportAndScissorRect(payload.framebuffer->getFramebufferInfo().getViewport());
    state.addBindingSet(binding_set);
    return std::move(state);
}

struct Draw : public Step {
    Payload payload;

    Draw() = delete;

    std::list<std::string> inputs;

    explicit Draw(
        const Context& ctx,
        Payload&& payload,
        std::list<std::string>&& inputs
        )
        : Step(ctx,
               "RunDrawCommand",
               "use_stencil, use_dynamic_ref, back_face, static_stencil, dynamic_stencil_0, dynamic_stencil_1, stencil_clean, z",
               inputs.front()),
          payload(std::move(payload)) {
        inputs.pop_front();
        this->inputs = std::move(inputs);
    }

    StepFuture run(std::string input) override {
        std::stringstream ss(input);
        bool use_stencil, use_dynamic_ref, back_face;
        uint8_t static_stencil, dynamic_stencil_0, dynamic_stencil_1, stencil_clean;
        float z;

        parse_input(input,
                    &use_stencil,
                    &use_dynamic_ref,
                    &back_face,
                    &static_stencil,
                    &dynamic_stencil_0,
                    &dynamic_stencil_1,
                    &stencil_clean,
                    &z);

        std::cout << "Using stencil: " << std::boolalpha << use_stencil << "\nuse_dynamic_ref: " << use_dynamic_ref <<
            "\nback_face: " << back_face << "\nstatic stencil: " << (int)static_stencil << "\ndynamic stencil 0: " <<
            (int)dynamic_stencil_0 << "\ndynamic stencil 1:" << (int)dynamic_stencil_1 << "\nstencil_clean: " << (int)
            stencil_clean << "\nz: " << z <<
            std::endl;

        nvrhi::DrawArguments args{};
        args.vertexCount = 3;
        auto binding_layout = create_graphics_binding_layout(context.nvrhiDevice);
        auto pipeline = create_graphics_pipeline(context.nvrhiDevice,
                                                 payload,
                                                 binding_layout,
                                                 use_stencil,
                                                 use_dynamic_ref,
                                                 back_face,
                                                 static_stencil);
        const nvrhi::FramebufferAttachment& colorA = payload.framebuffer->getDesc().colorAttachments[0];
        const nvrhi::FramebufferAttachment& depthA = payload.framebuffer->getDesc().depthAttachment;

        payload.command_list->open();
        payload.command_list->clearTextureFloat(colorA.texture, colorA.subresources, {0.1f, 0.3f, 0.6f, 1.0f});
        payload.command_list->clearDepthStencilTexture(depthA.texture,
                                                       depthA.subresources,
                                                       true,
                                                       1.0f,
                                                       true,
                                                       stencil_clean);

        payload.command_list->beginTrackingBufferState(payload.constant_buffer, nvrhi::ResourceStates::ConstantBuffer);
        auto binding_set = create_graphics_binding_set(context, payload, binding_layout);
        auto state = create_graphics_state(context, payload, pipeline, binding_set);
        state.framebuffer = payload.framebuffer;
        Uniform uniform;
        state.setDynamicStencilRefValue(dynamic_stencil_0);

        uniform.color = {1, 0, 0, 1};
        uniform.transform = transform(-0.2, 0.0);
        payload.command_list->writeBuffer(payload.constant_buffer, &uniform, sizeof(uniform));
        payload.command_list->setGraphicsState(state);
        payload.command_list->draw(args);

        // Dynamic ref shourdn't work here because it was set false during the pipeline setup
        state.setDynamicStencilRefValue(dynamic_stencil_1);
        uniform.color = {0, 1, 0, 1};
        // Even though the second triangle is upfront, the intersecting area should be screened at the stencil test
        uniform.transform = transform(0.2, z);
        payload.command_list->writeBuffer(payload.constant_buffer, &uniform, sizeof(uniform));
        payload.command_list->setGraphicsState(state);
        payload.command_list->draw(args);

        payload.command_list->close();
        context.nvrhiDevice->executeCommandList(payload.command_list);

        if (inputs.empty())
            return create_null_step();
        else
            return create_step_immediately<Draw>(context, std::move(payload), std::move(inputs));
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

    return create_step_immediately<Draw>(context,
                                         Payload{
                                             std::move(colorTexture),
                                             std::move(depthTexture),
                                             std::move(vertex),
                                             std::move(pixel),
                                             std::move(framebuffer),
                                             std::move(constant_buffer),
                                             std::move(command_list)
                                         },
                                         std::list<std::string>{
                                             "false false false 0b00001111 0b00111111 0b11111100 0 -0.1",
                                             "true false false 0b00001111 0b00111111 0b11111100 0 -0.1",
                                             "true true false 0b00001111 0b00111111 0b11111100 0 -0.1",
                                             "true true true 0b00001111 0b00111111 0b11111100 255 -0.1",
                                             "true false true 0b00001111 0b00111111 0b11111100 255 -0.1",
                                             "true false true 0b00001111 0b00111111 0b11111100 255 0.1",
                                         });
}