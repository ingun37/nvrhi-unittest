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
struct Payload {
    nvrhi::BufferHandle vertex_buffer;
    nvrhi::ShaderHandle compute;
    nvrhi::CommandListHandle commandList;
};

using vertex_t = std::array<float, 4>;
constexpr uint32_t horizontal_count = 2;
constexpr uint32_t vertical_count = 64;
constexpr uint32_t vertex_count = horizontal_count * vertical_count;

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

struct Compute : public Step {
    Payload payload;

    Compute() = delete;

private:
public:
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
        return create_null_step();
    }
};
}

StepFuture DynamicRender::run(std::string input) {
    std::string shader_dir = SCENARIO_SHADERS_OUTPUT_DIR;
    std::string path = shader_dir + "/generate-geometry" + extension();
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

    nvrhi::ShaderDesc computeDesc{};
    computeDesc.setEntryName("cs");
    computeDesc.setShaderType(nvrhi::ShaderType::Compute);
    auto compute = context.nvrhiDevice->createShader(computeDesc, fileData.data(), fileData.size());

    nvrhi::BufferDesc bd{};
    bd.setCanHaveUAVs(true);
    bd.setByteSize(sizeof(vertex_t) * vertex_count);
    auto vertex_buffer = context.nvrhiDevice->createBuffer(bd);
    auto command = context.nvrhiDevice->createCommandList();

    return create_step_immediately<Compute>(context,
                                            Payload{
                                                std::move(vertex_buffer),
                                                std::move(compute),
                                                std::move(command),
                                            });
}