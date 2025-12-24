//
// Created by Ingun Jon on 12/24/25.
//

#include "Triangle.h"
#include <stdexcept>
#include <fstream>

nvrhi::ShaderHandle CreateShader(const char* fileName,
                                 const char* entryPoint,
                                 const nvrhi::ShaderType type,
                                 const nvrhi::DeviceHandle& m_Device
    ) {
    nvrhi::ShaderDesc desc;
    if (desc.debugName.empty())
        desc.debugName = fileName;
    desc.entryName = entryPoint;
    desc.shaderType = type;

    std::ifstream file(fileName, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        // Handle error: file not found
        return nullptr;
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<uint8_t> byteCode(size);
    if (!file.read(reinterpret_cast<char*>(byteCode.data()), size)) {
        // Handle error: read failed
        return nullptr;
    }
    return m_Device->createShader(desc, byteCode.data(), byteCode.size());
}

Triangle::Triangle(Context&& webGpu)
    : App(std::move(webGpu), "Triangle") {
}

nvrhi::BufferDesc Triangle::CreateStaticConstantBufferDesc(
    uint32_t byteSize,
    const char* debugName) {
    nvrhi::BufferDesc constantBufferDesc;
    constantBufferDesc.byteSize = byteSize;
    constantBufferDesc.debugName = debugName;
    constantBufferDesc.isConstantBuffer = true;
    constantBufferDesc.isVolatile = false;
    return constantBufferDesc;
}

AppPtr Triangle::run() {
    auto m_ConstantBuffer = context.nvrhiDevice->createBuffer(
        CreateStaticConstantBufferDesc(sizeof(ConstantBufferEntry), "ConstantBuffer")
        .setInitialState(nvrhi::ResourceStates::ConstantBuffer).setKeepInitialState(true));

    nvrhi::VertexAttributeDesc attributes[] = {
        nvrhi::VertexAttributeDesc()
        .setName("position")
        .setFormat(nvrhi::Format::RGB32_FLOAT)
        .setOffset(0)
        .setBufferIndex(0)
        .setElementStride(sizeof(Vertex)),
        nvrhi::VertexAttributeDesc()
        .setName("uv")
        .setFormat(nvrhi::Format::RG32_FLOAT)
        .setOffset(0)
        .setBufferIndex(1)
        .setElementStride(sizeof(Vertex)),
    };

    std::string shaderFilePath =
        "/Users/ingun/CLionProjects/nvrhi-unit-test/cmake-build-webgpu/shaders/generated_wgsl/shader.wgsl";
    auto m_VertexShader = CreateShader(shaderFilePath.c_str(),
                                       "vertexMain",
                                       nvrhi::ShaderType::Vertex,
                                       context.nvrhiDevice);

    auto m_PixelShader = CreateShader(shaderFilePath.c_str(),
                                      "fragmentMain",
                                      nvrhi::ShaderType::Pixel,
                                      context.nvrhiDevice);

    auto m_InputLayout = context.nvrhiDevice->createInputLayout(attributes,
                                                                uint32_t(std::size(attributes)),
                                                                m_VertexShader);
    auto m_CommandList = context.nvrhiDevice->createCommandList();

    throw std::runtime_error("not implemented");
}