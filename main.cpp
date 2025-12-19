#include <gtest/gtest.h>
#include <webgpu/webgpu_cpp.h>
#include <webgpu/webgpu_cpp_print.h>

#include <cstdlib>
#include <iostream>
// Demonstrate some basic assertions.
TEST(HelloTest, BasicAssertions) {
    // Expect two strings not to be equal.
    EXPECT_STRNE("hello", "world");
    // Expect equality.
    EXPECT_EQ(7 * 6, 42);
}

TEST(WgpuTest, BasicAssertions) {
    static constexpr auto kTimedWaitAny = wgpu::InstanceFeatureName::TimedWaitAny;
    wgpu::InstanceDescriptor instanceDescriptor{
        .requiredFeatureCount = 1,
        .requiredFeatures = &kTimedWaitAny
      };

    wgpu::Instance instance = wgpu::CreateInstance(&instanceDescriptor);

    EXPECT_NE(instance, nullptr);
    // Synchronously request the adapter.
    wgpu::RequestAdapterOptions options = {};
    wgpu::Adapter adapter;

    auto callback = [](wgpu::RequestAdapterStatus status, wgpu::Adapter adapter, const char *message, void *userdata) {
        if (status != wgpu::RequestAdapterStatus::Success) {
            std::cerr << "Failed to get an adapter:" << message;
            return;
        }
        *static_cast<wgpu::Adapter *>(userdata) = adapter;
    };


    auto callbackMode = wgpu::CallbackMode::WaitAnyOnly;
    void *userdata = &adapter;
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


    wgpu::DeviceDescriptor deviceDesc = {};
    deviceDesc.nextInChain = nullptr;
    deviceDesc.SetDeviceLostCallback(
        wgpu::CallbackMode::AllowSpontaneous,
        [](const wgpu::Device&, wgpu::DeviceLostReason reason, wgpu::StringView message) {
            const char* reasonName = "";
            switch (reason) {
                case wgpu::DeviceLostReason::Unknown:
                    reasonName = "Unknown";
                    break;
                case wgpu::DeviceLostReason::Destroyed:
                    reasonName = "Destroyed";
                    break;
                case wgpu::DeviceLostReason::CallbackCancelled:
                    reasonName = "CallbackCancelled";
                    break;
                case wgpu::DeviceLostReason::FailedCreation:
                    reasonName = "FailedCreation";
                    break;
                default:
                    throw std::runtime_error("Unexpected device lost reason");
            }
            std::cerr << "Device lost because of " << reasonName << ": " << message;
        });
    deviceDesc.SetUncapturedErrorCallback(
        [](const wgpu::Device&, wgpu::ErrorType type, wgpu::StringView message) {
            const char* errorTypeName = "";
            switch (type) {
                case wgpu::ErrorType::Validation:
                    errorTypeName = "Validation";
                    break;
                case wgpu::ErrorType::OutOfMemory:
                    errorTypeName = "Out of memory";
                    break;
                case wgpu::ErrorType::Internal:
                    errorTypeName = "Internal";
                    break;
                case wgpu::ErrorType::Unknown:
                    errorTypeName = "Unknown";
                    break;
                default:
                    throw std::runtime_error("Unexpected error type");
            }
            std::cerr << errorTypeName << " error: " << message;
        });

    wgpu::Device device = nullptr;
    wgpu::Queue queue = nullptr;

    // Synchronously create the device
    instance.WaitAny(
        adapter.RequestDevice(
            &deviceDesc, wgpu::CallbackMode::WaitAnyOnly,
            [&device,&queue](wgpu::RequestDeviceStatus status, wgpu::Device dv, wgpu::StringView message) {
                if (status != wgpu::RequestDeviceStatus::Success) {
                    std::cerr << "Failed to get an device: " << message;
                    throw std::runtime_error("Failed to get an device");
                }
                device = std::move(dv);
                queue = device.GetQueue();
            }),
        UINT64_MAX);

    EXPECT_NE(device, nullptr);
    EXPECT_NE(queue, nullptr);
}