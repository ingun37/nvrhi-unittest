//
// Created by Ingun Jon on 1/6/26.
//
#include "wgpu-util.h"

#include <iostream>
#include <fstream>

void request_adapter(const wgpu::Instance &instance, std::function<void(wgpu::Adapter &&)> callback) {
    std::cout << "Requesting adapter" << std::endl;
    wgpu::RequestAdapterOptions adapterOptions = {};

    adapterOptions.nextInChain = nullptr;
    adapterOptions.backendType = wgpu::BackendType::WebGPU;

    instance.RequestAdapter(
        &adapterOptions, wgpu::CallbackMode::AllowSpontaneous,
        [cb = std::move(callback)](wgpu::RequestAdapterStatus status, wgpu::Adapter adapter,
                                   wgpu::StringView message) {
            if (status != wgpu::RequestAdapterStatus::Success) {
                std::cerr << "Failed to get an adapter: " << message.data;
                return;
            } else {
                std::cout << "Adapter is created" << std::endl;
                cb(std::move(adapter));
            }
        });
}

void request_device(const wgpu::Adapter &adapter, std::function<void(wgpu::Device &&)> callback) {
    wgpu::DeviceDescriptor deviceDesc = {};
    deviceDesc.nextInChain = nullptr;
    std::cout << "Requesting device" << std::endl;
    adapter.RequestDevice(
        &deviceDesc, wgpu::CallbackMode::AllowSpontaneous,
        [cb = std::move(callback)](wgpu::RequestDeviceStatus status, wgpu::Device dv,
                                   wgpu::StringView message) {
            if (status != wgpu::RequestDeviceStatus::Success) {
                std::cerr << "Failed to get an device: " << message.data;
                throw std::runtime_error("Failed to get an device");
            }
            std::cout << "Device is created" << std::endl;
            cb(std::move(dv));
        });
}

wgpu::Surface *create_surface(wgpu::Instance *instance) {
    std::cout << "Creating surface" << std::endl;

    wgpu::EmscriptenSurfaceSourceCanvasHTMLSelector fromCanvasHTMLSelector;
    fromCanvasHTMLSelector.nextInChain = nullptr;
    fromCanvasHTMLSelector.sType =
            wgpu::SType::EmscriptenSurfaceSourceCanvasHTMLSelector;
    fromCanvasHTMLSelector.selector.data = "#canvas";
    fromCanvasHTMLSelector.selector.length = 7;

    wgpu::SurfaceDescriptor surfaceDescriptor;
    surfaceDescriptor.nextInChain = &fromCanvasHTMLSelector;
    surfaceDescriptor.label.data = "canvas_surface";
    surfaceDescriptor.label.length = 14;
    return new wgpu::Surface(instance->CreateSurface(&surfaceDescriptor));
}

void configure_surface(
    wgpu::Adapter *adapter,
    wgpu::Device *device,
    wgpu::Surface *surface,
    int width, int height
) {
    std::cout << "Configuring surface" << std::endl;
    wgpu::SurfaceCapabilities capabilities;
    surface->GetCapabilities(*adapter, &capabilities);
    wgpu::SurfaceConfiguration config = {};
    config.device = *device;
    config.format = capabilities.formats[0];
    config.width = width;
    config.height = height;
    config.presentMode = capabilities.presentModes[0];
    surface->Configure(&config);
}


namespace nvrhi {
    namespace utils {
        nvrhi::BufferDesc CreateStaticConstantBufferDesc(
            uint32_t byteSize,
            const char *debugName) {
            nvrhi::BufferDesc constantBufferDesc;
            constantBufferDesc.byteSize = byteSize;
            constantBufferDesc.debugName = debugName;
            constantBufferDesc.isConstantBuffer = true;
            constantBufferDesc.isVolatile = false;
            return constantBufferDesc;
        }
    }
}


struct ConstantBufferEntry {
    dm::float4x4 viewProjMatrix;
    float padding[16 * 3];
};

nvrhi::ShaderHandle CreateShader(const char *fileName,
                                 const char *entryPoint,
                                 const nvrhi::ShaderType type,
                                 const nvrhi::DeviceHandle &m_Device
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
    if (!file.read(reinterpret_cast<char *>(byteCode.data()), size)) {
        // Handle error: read failed
        return nullptr;
    }
    return m_Device->createShader(desc, byteCode.data(), byteCode.size());
}

nvrhi::ShaderHandle *create_vertex_shader(const std::string &shaderFilePath, nvrhi::DeviceHandle &nvrhi_device) {
    return new nvrhi::ShaderHandle(CreateShader(shaderFilePath.c_str(),
                                                "vertexMain",
                                                nvrhi::ShaderType::Vertex,
                                                nvrhi_device));
}

nvrhi::ShaderHandle *create_pixel_shader(const std::string &shaderFilePath, nvrhi::DeviceHandle &nvrhi_device) {
    return new nvrhi::ShaderHandle(CreateShader(shaderFilePath.c_str(),
                                                "fragmentMain",
                                                nvrhi::ShaderType::Pixel,
                                                nvrhi_device));
}

nvrhi::StagingTextureHandle *create_staging(const std::string &image_path, nvrhi::DeviceHandle &device) {
    nvrhi::TextureDesc stagingTextureDesc{};
    const int width = 100;
    const int height = 100;
    stagingTextureDesc.width = width;
    stagingTextureDesc.height = height;
    stagingTextureDesc.format = nvrhi::Format::RGBA8_UNORM;
    auto staging = device->createStagingTexture(stagingTextureDesc, nvrhi::CpuAccessMode::Write);

    nvrhi::TextureSlice slice{};
    slice = slice.resolve(staging->getDesc());
    const size_t pixelPitch = 4 * 8;
    const uint32_t imageRowPitch = width * pixelPitch;
    size_t pitch;
    auto mapPtr = static_cast<uint8_t *>(
        device->mapStagingTexture(
            staging,
            slice,
            nvrhi::CpuAccessMode::Write,
            &pitch));

    const uint32_t pixelSize = imageRowPitch / width;

    std::cout << "Pitch: " << pitch << std::endl;
    auto noise = std::make_unique<uint8_t[]>(imageRowPitch * height);
    for (int i = 0; i < width; ++i) {
        for (int j = 0; j < height; ++j) {
            noise[j * imageRowPitch + i * pixelPitch] = i % 256;
            noise[j * imageRowPitch + i * pixelPitch + 1] = j % 256;
            noise[j * imageRowPitch + i * pixelPitch + 2] = 200;
            noise[j * imageRowPitch + i * pixelPitch + 3] = 200;
        }
    }
    for (uint32_t i = 0; i < height; ++i) {
        memcpy(mapPtr + (i * staging->getDesc().width * pixelSize),
               noise.get() + (i * imageRowPitch),
               pitch);
    }
    device->unmapStagingTexture(staging);
    return new nvrhi::StagingTextureHandle(std::move(staging));
}

nvrhi::BufferHandle* create_buffer(nvrhi::DeviceHandle& device) {
    nvrhi::BufferDesc buffer_desc{};
    const int width = 100;
    const int height = 100;
    const size_t pixelPitch = 4 * 8;
    const uint32_t imageRowPitch = width * pixelPitch;
    buffer_desc.format = nvrhi::Format::RGBA8_UNORM;
    buffer_desc.byteSize = imageRowPitch * height;
    buffer_desc.cpuAccess = nvrhi::CpuAccessMode::Read;

    auto buffer = device->createBuffer(buffer_desc);

    return new nvrhi::BufferHandle(std::move(buffer));
}

void read_buffer(nvrhi::DeviceHandle& device, nvrhi::BufferHandle& buffer, std::function<void(const void*)> callback) {
    device->mapBufferAsync(buffer, nvrhi::CpuAccessMode::Read, callback);
}