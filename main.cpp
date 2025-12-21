#include <gtest/gtest.h>
#include <nvrhi/nvrhi.h>
#include <nvrhi/webgpu.h>
#include <webgpu/webgpu_cpp.h>
#include <webgpu/webgpu_cpp_print.h>

#include <cstdlib>
#include <iostream>

#include "util.h"
#include <GLFW/glfw3.h>
#include "metal.h"
#include "Image/stb_image.h"
#include "helper.h"
// Demonstrate some basic assertions.
TEST(HelloTest, BasicAssertions) {
    // Expect two strings not to be equal.
    EXPECT_STRNE("hello", "world");
    // Expect equality.
    EXPECT_EQ(7 * 6, 42);
}

TEST(DawnTest, BasicAssertions) {
    static constexpr auto kTimedWaitAny = wgpu::InstanceFeatureName::TimedWaitAny;
    wgpu::InstanceDescriptor instanceDescriptor{.requiredFeatureCount = 1, .requiredFeatures = &kTimedWaitAny};

    wgpu::Instance instance = wgpu::CreateInstance(&instanceDescriptor);

    EXPECT_NE(instance, nullptr);
    // Synchronously request the adapter.
    wgpu::RequestAdapterOptions options = {};
    wgpu::Adapter adapter;

    auto callback = [](wgpu::RequestAdapterStatus status, wgpu::Adapter adapter, const char* message, void* userdata) {
        if (status != wgpu::RequestAdapterStatus::Success) {
            std::cerr << "Failed to get an adapter:" << message;
            return;
        }
        *static_cast<wgpu::Adapter*>(userdata) = adapter;
    };

    auto callbackMode = wgpu::CallbackMode::WaitAnyOnly;
    void* userdata = &adapter;
    instance.WaitAny(instance.RequestAdapter(&options, callbackMode, callback, userdata), UINT64_MAX);
    EXPECT_NE(adapter, nullptr);

    wgpu::DawnAdapterPropertiesPowerPreference power_props{};

    wgpu::AdapterInfo info{};
    info.nextInChain = &power_props;

    adapter.GetInfo(&info);
    std::cout << "VendorID: " << std::hex << info.vendorID << std::dec << "\n";
    std::cout << "Vendor: " << info.vendor << "\n";
    std::cout << "Architecture: " << info.architecture << "\n";
    std::cout << "DeviceID: " << std::hex << info.deviceID << std::dec << "\n";
    std::cout << "Name: " << info.device << "\n";
    std::cout << "Driver description: " << info.description << "\n";

    wgpu::Device device = create_device(instance, adapter);
    wgpu::Queue queue = device.GetQueue();

    EXPECT_NE(device, nullptr);
    EXPECT_NE(queue, nullptr);
}

TEST(NvrhiTest, BasicAssertions) {
    wgpu::InstanceDescriptor instanceDescriptor = nvrhi::webgpu::utils::create_instance_descriptor();
    wgpu::Instance instance = wgpu::CreateInstance(&instanceDescriptor);
    wgpu::Adapter adapter = nvrhi::webgpu::utils::create_adapter(
        instance,
        nvrhi::webgpu::utils::create_adapter_option(wgpu::BackendType::Metal, wgpu::AdapterType::IntegratedGPU));
    wgpu::Device device = create_device(instance, adapter);
    wgpu::Queue queue = device.GetQueue();
    auto nvrhiDevice = nvrhi::webgpu::createDevice({device, queue});

    size_t stagingTextureRowPitch = 0;
    uint32_t copyWidth = 1024;
    uint32_t copyHeight = 1024;
    uint32_t copyDepth = 1;
    nvrhi::Format fmt = nvrhi::Format::RGBA8_UNORM;
    nvrhi::TextureDimension dimension = nvrhi::TextureDimension::Texture2D;
    nvrhi::TextureDesc stagingTextureDesc;
    stagingTextureDesc.width = copyWidth;
    stagingTextureDesc.height = copyHeight;
    stagingTextureDesc.depth = copyDepth;
    stagingTextureDesc.arraySize = 1;
    stagingTextureDesc.mipLevels = 1;
    stagingTextureDesc.format = fmt;
    stagingTextureDesc.dimension = dimension;
    stagingTextureDesc.isShaderResource = false;
    stagingTextureDesc.initialState = nvrhi::ResourceStates::CopySource;
    stagingTextureDesc.keepInitialState = true;
    nvrhi::TextureSlice stagingTextureSlice;
    stagingTextureSlice.x = stagingTextureSlice.y = stagingTextureSlice.z = 0;
    stagingTextureSlice.width = copyWidth;
    stagingTextureSlice.height = copyHeight;
    stagingTextureSlice.depth = copyDepth;
    stagingTextureSlice.mipLevel = stagingTextureSlice.arraySlice = 0;

    nvrhi::StagingTextureHandle stagingTexture =
        nvrhiDevice->createStagingTexture(stagingTextureDesc, nvrhi::CpuAccessMode::Write);

    void* mapPointer = nvrhiDevice->mapStagingTexture(
        stagingTexture,
        stagingTextureSlice,
        nvrhi::CpuAccessMode::Write,
        &stagingTextureRowPitch);

    EXPECT_NE(mapPointer, nullptr);
    // const uint8_t* sliceData = textureData.m_dataBlob.data() + (iterArraySlice * textureData.m_slicePitch);
    //
    // memcpy(mapPointer, sliceData, dataCopySize);
    nvrhiDevice->unmapStagingTexture(stagingTexture);

    nvrhi::TextureSlice destTextureSlice;
    destTextureSlice.x = 0;
    destTextureSlice.y = 0;
    destTextureSlice.z = 0;
    destTextureSlice.width = copyWidth;
    destTextureSlice.height = copyHeight;
    destTextureSlice.depth = copyDepth;
    destTextureSlice.mipLevel = 1;
    destTextureSlice.arraySlice = 0;

    auto commandList = nvrhiDevice->createCommandList();
    // commandList->copyTexture(targetTexture->m_textureHandle,
    //                          destTextureSlice,
    //                          stagingTexture,
    //                          stagingTextureSlice);
}
