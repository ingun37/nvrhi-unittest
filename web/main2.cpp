#include <emscripten/bind.h>

#include <webgpu/webgpu_cpp.h>

#include <iostream>

#include <coroutine>
#include <functional>
#include <random>

struct P {
    wgpu::Instance instance;
    wgpu::Adapter *adapter = nullptr;

    P() = delete;

    explicit P(wgpu::Instance instance) : instance(instance) {
    }
};

void request_adapter(P p) {
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


auto switch_to_new_thread(P out, wgpu::Instance &instance, wgpu::Adapter *adapter) {
    struct awaitable {
        P p_out;
        wgpu::Instance &instance;
        wgpu::Adapter *adapter;

        awaitable() = delete;

        awaitable(P p_out, wgpu::Instance &instance, wgpu::Adapter *adapter) : p_out{p_out}, instance{instance},
                                                                               adapter{adapter} {
        }

        bool await_ready() { return false; }

        bool await_suspend(std::coroutine_handle<> h) {
            request_adapter(p_out);
            return false;
        }

        void await_resume() {
        }
    };
    return awaitable{out, instance, adapter};
}

struct task {
    struct promise_type {
        task get_return_object() { return task{this}; }
        std::suspend_always initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }

        void return_void() {
        }

        void unhandled_exception() {
        }
    };

    using Handle = std::coroutine_handle<promise_type>;
    Handle hdl;

    task() = delete;

    explicit task(promise_type *pt) : hdl{Handle::from_promise(*pt)} {
    }
};

task resuming_on_new_thread(P out, wgpu::Instance &instance, wgpu::Adapter *adapter) {
    std::cout << "resuming_on_new_thread start" << std::endl;
    co_await switch_to_new_thread(out, instance, adapter);
    std::cout << "resuming_on_new_thread done" << std::endl;
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

    wgpu::Adapter adapter;
    auto t = resuming_on_new_thread(p, instance, &adapter);

    while (!t.hdl.done()) {
        std::cout << "resume" << std::endl;
        t.hdl.resume();
    }

    std::cout << "exit" << std::endl;
    // // Synchronously request the adapter.
    // wgpu::RequestAdapterOptions options = {};
    // wgpu::Adapter adapter;
    //
    // auto callback = [](wgpu::RequestAdapterStatus status, wgpu::Adapter
    //                    adapter, const char *message, void *userdata) {
    //     if (status != wgpu::RequestAdapterStatus::Success) {
    //         std::cerr << "Failed to get an adapter:" << message;
    //         return;
    //     }
    //     std::cout << "Adapter is created" << std::endl;
    //     *static_cast<wgpu::Adapter *>(userdata) = adapter;
    // };
    //
    // auto callbackMode = wgpu::CallbackMode::AllowSpontaneous;
    // void *userdata = &adapter;
    // std::cout << "Requesting adapter" << std::endl;
    //
    // instance.RequestAdapter(&options, callbackMode, callback, userdata);
    // while (adapter == nullptr) {
    //     std::cerr << "waiting\n";
    // }
    // std::cout << "Adapter is created 2" << std::endl;
    //
    //
    // wgpu::AdapterInfo info{};
    //
    // adapter.GetInfo(&info);
    // std::cout << "VendorID: " << std::hex << info.vendorID << std::dec << "\n";
    // std::cout << "Vendor: " << info.vendor.data << "\n";
    // std::cout << "Architecture: " << info.architecture.data << "\n";
    // std::cout << "DeviceID: " << std::hex << info.deviceID << std::dec << "\n";
    // std::cout << "Name: " << info.device.data << "\n";
    // std::cout << "Driver description: " << info.description.data << "\n";
    // return EXIT_SUCCESS;
}
