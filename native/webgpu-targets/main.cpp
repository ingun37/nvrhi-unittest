#include <gtest/gtest.h>
#include <nvrhi/nvrhi.h>
#include <webgpu/webgpu_cpp.h>
#include <webgpu/webgpu_cpp_print.h>

#include <iostream>

#include <GLFW/glfw3.h>
#include "webgpu-util.h"
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
    device.Destroy();
}


