#include <coroutine>
#include <iostream>
#include <stdexcept>
#include <thread>
#include <webgpu/webgpu_cpp.h>
#include <pthread.h>
#include <unistd.h>


wgpu::Instance create_instance() {
    wgpu::InstanceDescriptor instanceDescriptor{};

    wgpu::Instance instance = wgpu::CreateInstance(&instanceDescriptor);
    if (instance == nullptr) {
        std::cerr << "Instance creation failed!\n";
        throw std::runtime_error("Failed to create instance");
    }
    return instance;
}

struct UserData {
    wgpu::Adapter adapter = nullptr;

    ~UserData() {
        std::cout << "~UserData()" << std::endl;
    }
};

void print_adapter_info(wgpu::Adapter &adapter) {
    std::cout << "adapter address1:" << adapter.Get() << std::endl;
    wgpu::AdapterInfo info{};
    adapter.GetInfo(&info);
    std::cout << "VendorID: " << std::hex << info.vendorID << std::dec << "\n";
    std::cout << "Vendor: " << info.vendor.data << "\n";
    std::cout << "Architecture: " << info.architecture.data << "\n";
    std::cout << "DeviceID: " << std::hex << info.deviceID << std::dec << "\n";
    std::cout << "Name: " << info.device.data << "\n";
    std::cout << "Driver description: " << info.description.data << "\n";
}


void adapter_callback(wgpu::RequestAdapterStatus status, wgpu::Adapter adapter,
                      const char *message, void *ud) {
    if (status != wgpu::RequestAdapterStatus::Success) {
        std::cerr << "Failed to get an adapter:" << message;
        return;
    }
    auto user_data = static_cast<UserData *>(ud);
    std::cout << "Adapter is created: " << adapter.Get() << std::endl;
    user_data->adapter = std::move(adapter);
}

struct Input {
    wgpu::Instance instance{};
    UserData user_data{};
};

void *create_adapter(void *arg) {
    sleep(1);

    wgpu::RequestAdapterOptions options = {};

    Input *input = (Input *) (arg);
    input->instance.RequestAdapter(&options,
                                   wgpu::CallbackMode::AllowSpontaneous,
                                   adapter_callback, (void *) &(input->user_data));
    return nullptr;
}

void *wait(void *user_data) {
    sleep(1);

    std::cout << "wait" << std::endl;
    while (((UserData *) user_data)->adapter == nullptr);
    std::cout << "wait done. resume!" << std::endl;
    return nullptr;
}


int main() {
    auto instance = create_instance();
    Input input{instance};
    // pthread_t thread_id_0;
    // pthread_create(&thread_id_0, nullptr, create_adapter, &(input));
    create_adapter(&input);
    // std::cout << "Join: " << thread_id_0 << std::endl;
    // pthread_join(thread_id_0, nullptr);

    pthread_t thread_id;
    pthread_create(&thread_id, nullptr, wait, &(input.user_data));

    std::cout << "Join: " << thread_id << std::endl;
    pthread_join(thread_id, nullptr);

    print_adapter_info(input.user_data.adapter);
    std::cout << "Exit main thread" << std::endl;
}
