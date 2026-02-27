//
// Created by Ingun Jon on 2/26/26.
//

#include "MovableTriangle.h"
#include <iostream>
#include <fstream>
#include <list>

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
    nvrhi::TextureHandle colorTexture;
    nvrhi::TextureHandle depthTexture;
    nvrhi::ShaderHandle vertex;
    nvrhi::ShaderHandle pixel;
    nvrhi::FramebufferHandle framebuffer;
    nvrhi::BufferHandle constantBuffer;
    nvrhi::CommandListHandle commandList;
};

nvrhi::BindingLayoutHandle make_binding_layout(const nvrhi::DeviceHandle& device) {
    nvrhi::BindingLayoutItem bli{};
    bli.setSlot(0);
    bli.setType(nvrhi::ResourceType::ConstantBuffer);
    nvrhi::BindingLayoutDesc bld{};
    bld.setVisibility(nvrhi::ShaderType::AllGraphics);
    bld.addItem(bli);
    return device->createBindingLayout(bld);
}

nvrhi::GraphicsPipelineHandle make_pipeline(const nvrhi::DeviceHandle& device,
                                            const Payload& payload,
                                            const nvrhi::BindingLayoutHandle& binding_layout) {
    nvrhi::GraphicsPipelineDesc gpd;
    gpd.VS = payload.vertex;
    gpd.PS = payload.pixel;
    gpd.primType = nvrhi::PrimitiveType::TriangleList;
    gpd.renderState.depthStencilState.depthTestEnable = true;
    gpd.renderState.depthStencilState.depthWriteEnable = true;
    gpd.renderState.blendState.targets[0].blendEnable = true;
    gpd.renderState.blendState.targets[0].srcBlend = nvrhi::BlendFactor::SrcAlpha;
    gpd.renderState.blendState.targets[0].destBlend = nvrhi::BlendFactor::OneMinusSrcAlpha;
    gpd.renderState.blendState.targets[0].blendOp = nvrhi::BlendOp::Add;
    gpd.renderState.blendState.targets[0].srcBlendAlpha = nvrhi::BlendFactor::One;
    gpd.renderState.blendState.targets[0].destBlendAlpha = nvrhi::BlendFactor::OneMinusSrcAlpha;
    gpd.renderState.blendState.targets[0].blendOpAlpha = nvrhi::BlendOp::Add;

    gpd.addBindingLayout(binding_layout);
    return device->createGraphicsPipeline(gpd, payload.framebuffer);
}

nvrhi::GraphicsState make_state(const Context& context,
                                const Payload& payload,
                                std::list<nvrhi::RefCountPtr<nvrhi::IResource> >& hold) {
    auto binding_layout = make_binding_layout(context.nvrhiDevice);
    hold.emplace_back(binding_layout);
    auto pipeline = make_pipeline(context.nvrhiDevice, payload, binding_layout);
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

void run_(std::string input, const Payload& payload, const Context& context, float x, float4 color) {
    int depthBias = 0;
    try {
        depthBias = std::stoi(input);
    } catch (...) {
        std::cout << "Invalid input: " << input << ", using 0" << std::endl;
    }

    std::cout << "Using depth bias: " << depthBias << std::endl;
    std::list<nvrhi::RefCountPtr<nvrhi::IResource> > hold;
    auto state = make_state(context, payload, hold);
    payload.commandList->open();
    Uniform uniform{};
    uniform.color = color;
    uniform.transform[3][0] = x;
    payload.commandList->writeBuffer(payload.constantBuffer, &uniform, sizeof(uniform));
    payload.commandList->setGraphicsState(state);
    payload.commandList->draw({.vertexCount = 3});
    payload.commandList->close();

    context.nvrhiDevice->executeCommandList(payload.commandList);
}

struct DrawBase : public Step {
    Payload payload;

    DrawBase() = delete;

private:
    static std::string prompt() {
        return "Enter depth bias (float):";
    }

public:
    float x = 0;
    float4 color = {1, 0, 0, 1};

    DrawBase(const Context& ctx,
             Payload&& payload
        )
        : Step(ctx, "RunDrawCommand", prompt(), "0"),
          payload(std::move(payload)) {
    }

    StepFuture run(std::string input) override {
        run_(input, payload, context, x, color);

        return create_null_step();
    }
};

struct Draw2 : public DrawBase {
    using DrawBase::DrawBase;

    StepFuture run(std::string input) override {
        std::cout << "DRAW -- 2 --" << std::endl;
        x = 0.2;
        color = {0, 1, 0, 1};
        DrawBase::run(input);
        return create_null_step();
    }
};

struct Draw1 : public DrawBase {
    using DrawBase::DrawBase;

    StepFuture run(std::string input) override {
        std::cout << "DRAW -- 1 --" << std::endl;
        x = 0.0;
        color = {1, 0, 0, 1};
        DrawBase::run(input);
        return create_step_immediately<Draw2>(context, std::move(payload));
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

        return create_step_immediately<Draw1>(context, std::move(payload));
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