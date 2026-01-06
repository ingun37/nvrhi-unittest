#include <emscripten/bind.h>
#include <emscripten/val.h>

#include <webgpu/webgpu_cpp.h>

#include <iostream>
using namespace emscripten;
using namespace return_value_policy;

wgpu::Instance create_instance() {
  wgpu::InstanceDescriptor instanceDescriptor{};

  wgpu::Instance instance = wgpu::CreateInstance(&instanceDescriptor);
  if (instance == nullptr) {
    std::cerr << "Instance creation failed!\n";
    throw std::runtime_error("Failed to create instance");
  }
  return instance;
}

struct AdapterAwaiter {
  ~AdapterAwaiter() { std::cout << "~AdapterAwaiter()" << std::endl; }

  bool await_ready() {
    std::cout << "__await_ready()" << std::endl;
    return false;
  }

  void await_suspend(std::coroutine_handle<> handle) {
    other_handle = handle;
    std::cout << "__await_suspend()" << std::endl;

    wgpu::RequestAdapterOptions options = {};
    auto callback = [](wgpu::RequestAdapterStatus status, wgpu::Adapter adapter,
                       const char *message, void *ud) {
      if (status != wgpu::RequestAdapterStatus::Success) {
        std::cerr << "Failed to get an adapter:" << message;
        return;
      }
      std::cout << "Adapter is created" << std::endl;

      static_cast<AdapterAwaiter *>(ud)->adapter = adapter;
      static_cast<AdapterAwaiter *>(ud)->other_handle.resume();
    };
    auto callbackMode = wgpu::CallbackMode::AllowProcessEvents;
    std::cout << "Requesting adapter" << std::endl;

    future =
        instance.RequestAdapter(&options, callbackMode, callback, (void *)this);
    std::cout << "returning void from await_suspend" << std::endl;
  }

  wgpu::Adapter await_resume() {
    std::cout << "__await_resume()" << std::endl;
    return adapter;
  }

  wgpu::Instance instance;
  wgpu::Adapter adapter;
  std::coroutine_handle<> other_handle;
  wgpu::Future future;
};

struct Co {
  struct promise_type {
    void unhandled_exception() noexcept {}

    Co get_return_object() {
      std::cout << "__get_return_object();" << std::endl;
      return Co{this};
    }

    std::suspend_never initial_suspend() noexcept {
      std::cout << "__initial_suspend() -> never;" << std::endl;
      return {};
    }

    std::suspend_never yield_value() {
      std::cout << "__yield_value() -> never;" << std::endl;
      return {};
    }

    void return_value(wgpu::Adapter adapter) noexcept {
      std::cout << "__return_value(); setting _adapter to " << adapter.Get()
                << std::endl;
      this->_adapter = adapter;
    }

    std::suspend_always final_suspend() const noexcept { return {}; }
    wgpu::Adapter _adapter;
  };

  using Handle = std::coroutine_handle<promise_type>;
  Handle hdl;

  Co(promise_type *pt) : hdl{Handle::from_promise(*pt)} {}

  wgpu::Adapter listen() {
    if (not hdl.done()) {
      hdl.resume();
    }
    return hdl.promise()._adapter;
  }
};

val create_adapter(wgpu::Instance instance) {
  std::cout << "co_await" << std::endl;
  auto adapter = co_await AdapterAwaiter{instance};
  std::cout << "co_return" << std::endl;
  co_return adapter;
}

Co create_adapter_raw(wgpu::Instance instance) {
  std::cout << "co_await" << std::endl;
  auto adapter = co_await AdapterAwaiter{instance};
  std::cout << "co_return" << std::endl;
  co_return adapter;
}

wgpu::Adapter create_adapter2(wgpu::Instance instance) {
  std::cout << "> instance address" << instance.Get() << std::endl;
  auto co = create_adapter_raw(instance);
  std::cout << "> listening 1" << std::endl;
  auto adapter = co.listen();
  std::cout << "> adapter address0:" << adapter.Get() << std::endl;
  return adapter;
}

void print_adapter_info(wgpu::Adapter adapter) {
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

EMSCRIPTEN_BINDINGS(my_module) {
  class_<wgpu::Instance>("wgpu_Instance");
  function("create_instance", &create_instance, take_ownership());
  class_<wgpu::Adapter>("wgpu_adapter");
  function("create_adapter", &create_adapter, take_ownership());
  function("create_adapter2", &create_adapter2, take_ownership());
  function("print_adapter_info", &print_adapter_info);
}

int main(int argc, char *argv[]) {
  // wgpu::InstanceDescriptor instanceDescriptor{};
  //
  // wgpu::Instance instance = wgpu::CreateInstance(&instanceDescriptor);
  // if (instance == nullptr) {
  //     std::cerr << "Instance creation failed!\n";
  //     throw std::runtime_error("Failed to create instance");
  // }
  // // Synchronously request the adapter.
  // wgpu::RequestAdapterOptions options = {};
  // wgpu::Adapter adapter;
  //
  // auto callback = [](wgpu::RequestAdapterStatus status, wgpu::Adapter
  // adapter, const char *message, void *userdata) {
  //     if (status != wgpu::RequestAdapterStatus::Success) {
  //         std::cerr << "Failed to get an adapter:" << message;
  //         return;
  //     }
  //     std::cout << "Adapter is created" << std::endl;
  //     *static_cast<wgpu::Adapter *>(userdata) = adapter;
  // };
  //
  //
  // auto callbackMode = wgpu::CallbackMode::AllowSpontaneous;
  // void *userdata = &adapter;
  // std::cout << "Requesting adapter" << std::endl;
  //
  // instance.RequestAdapter(&options, callbackMode, callback, userdata);
  // if (adapter == nullptr) {
  //     std::cerr << "RequestAdapter failed!\n";
  //     return EXIT_FAILURE;
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
