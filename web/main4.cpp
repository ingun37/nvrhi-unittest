#include <webgpu/webgpu_cpp.h>
#include <iostream>

struct P {
    wgpu::Instance instance;
    wgpu::Adapter *adapter = nullptr;

    P() = delete;

    explicit P(wgpu::Instance instance) : instance(instance) {
    }

    ~P() {
        std::cout << "~P()" << std::endl;
    }
};

void request_adapter(P &p) {
    wgpu::RequestAdapterOptions options = {};
    auto callback = [](wgpu::RequestAdapterStatus status, wgpu::Adapter
                       adapter, const char *message, void *userdata) {
        if (status != wgpu::RequestAdapterStatus::Success) {
            std::cerr << "Failed to get an adapter:" << message;
            return;
        }
        std::cout << "Adapter is created" << std::endl;

        wgpu::AdapterInfo info{};

        adapter.GetInfo(&info);
        std::cout << "VendorID: " << std::hex << info.vendorID << std::dec << "\n";
        std::cout << "Vendor: " << info.vendor.data << "\n";
        std::cout << "Architecture: " << info.architecture.data << "\n";
        std::cout << "DeviceID: " << std::hex << info.deviceID << std::dec << "\n";
        std::cout << "Name: " << info.device.data << "\n";
        std::cout << "Driver description: " << info.description.data << "\n";
        P p = *static_cast<P *>(userdata);
        std::cout << "P.instance: " << p.instance.Get() << std::endl;
    };

    auto callbackMode = wgpu::CallbackMode::AllowSpontaneous;
    void *userdata = new P(p.instance);
    std::cout << "Requesting adapter" << std::endl;
    p.instance.RequestAdapter(&options, callbackMode, callback, userdata);
}


int main(int argc, char *argv[]) {
    std::cout << "start" << std::endl;
    wgpu::InstanceDescriptor instanceDescriptor{};

    wgpu::Instance instance = wgpu::CreateInstance(&instanceDescriptor);
    if (instance == nullptr) {
        std::cerr << "Instance creation failed!\n";
        throw std::runtime_error("Failed to create instance");
    }
    P p(instance);

    request_adapter(p);
    std::cout << "exit" << std::endl;
}
