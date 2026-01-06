#include <emscripten/bind.h>

#include <webgpu/webgpu_cpp.h>

#include <iostream>

#include <coroutine>
#include <functional>
#include <random>
#include <emscripten.h>
#include <emscripten/html5.h>

int main(int argc, char *argv[]) {
    std::cout << "start" << std::endl;
    wgpu::InstanceDescriptor instanceDescriptor{};
    wgpu::InstanceFeatureName features[] = {wgpu::InstanceFeatureName::TimedWaitAny};
    instanceDescriptor.requiredFeatureCount = 1;
    instanceDescriptor.requiredFeatures = features;
    wgpu::Instance instance = wgpu::CreateInstance(&instanceDescriptor);
    if (instance == nullptr) {
        std::cerr << "Instance creation failed!\n";
        throw std::runtime_error("Failed to create instance");
    }


    // // Synchronously request the adapter.
    wgpu::RequestAdapterOptions options = {};
    wgpu::Adapter adapter;
    auto callback = [](wgpu::RequestAdapterStatus status, wgpu::Adapter
                       adapter, const char *message, void *userdata) {
        if (status != wgpu::RequestAdapterStatus::Success) {
            std::cerr << "Failed to get an adapter:" << message;
            return;
        }
        std::cout << "Adapter is created" << std::endl;
        *static_cast<wgpu::Adapter *>(userdata) = adapter;
    };

    auto callbackMode = wgpu::CallbackMode::AllowSpontaneous;
    void *userdata = &adapter;
    std::cout << "Requesting adapter" << std::endl;

    instance.RequestAdapter(&options, callbackMode, callback, userdata);
    if (adapter == nullptr) {
        emscripten_sleep(100);
    }
    std::cout << "Adapter is created 2" << std::endl;


    wgpu::AdapterInfo info{};

    adapter.GetInfo(&info);
    std::cout << "VendorID: " << std::hex << info.vendorID << std::dec << "\n";
    std::cout << "Vendor: " << info.vendor.data << "\n";
    std::cout << "Architecture: " << info.architecture.data << "\n";
    std::cout << "DeviceID: " << std::hex << info.deviceID << std::dec << "\n";
    std::cout << "Name: " << info.device.data << "\n";
    std::cout << "Driver description: " << info.description.data << "\n";
    return EXIT_SUCCESS;
}
