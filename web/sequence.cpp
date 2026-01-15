#include <emscripten/html5.h>
#include <iostream>
#include <utility>
#include <nvrhi/nvrhi.h>
#include <nvrhi/webgpu.h>
#include <GLFW/glfw3.h>
#include <webgpu/webgpu_cpp.h>
#include <emscripten/bind.h>
#include <scenario/scenario.h>

#include "../native/scenario/ChooseApp.h"


using namespace emscripten;
static std::unique_ptr<std::string> g_input;


enum Stage { Wait, Next };

struct UserData {
    AppPtr app = nullptr;
    GLFWwindow* window;
    int width;
    int height;
    wgpu::Instance instance;
    wgpu::Adapter adapter = nullptr;
    wgpu::Device device = nullptr;

    Context context{nullptr};

    UserData() = delete;

    explicit UserData(GLFWwindow* window, int width, int height, wgpu::Instance&& instance)
        : window(window),
          width(width),
          height(height),
          instance(std::move(instance)) {
    }
};




void run_next_app(std::string input) {
    g_input = std::make_unique<std::string>(std::move(input));
}

EMSCRIPTEN_BINDINGS(my_module) {
    function("run_next_app", &run_next_app);
}

struct InitSurface : public App {
    UserData& user_data;

    InitSurface() = delete;

    InitSurface(UserData& user_data)
        : App(Context(nullptr),
              "Init Surface",
              "",
              ""),
          user_data(user_data) {
    }

    AppPtr run(std::string _) override {
        auto queue = user_data.device.GetQueue();
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
        auto surface = user_data.instance.CreateSurface(&surfaceDescriptor);

        std::cout << "Configuring surface" << std::endl;
        wgpu::SurfaceCapabilities capabilities;
        surface.GetCapabilities(user_data.adapter, &capabilities);
        wgpu::SurfaceConfiguration config = {};
        config.device = user_data.device;
        config.format = capabilities.formats[0];
        config.width = user_data.width;
        config.height = user_data.height;
        config.presentMode = capabilities.presentModes[0];
        surface.Configure(&config);
        user_data.context = Context{nvrhi::webgpu::createDevice({user_data.device, queue})};
        return std::make_unique<ChooseApp>(user_data.context);
    }
};

struct InitDevice : public App {
    UserData& user_data;

    InitDevice() = delete;

    InitDevice(UserData& user_data)
        : App(Context(nullptr), "Init Device", "", ""),
          user_data(user_data) {
    }

    AppPtr run(std::string _) override {
        wgpu::DeviceDescriptor deviceDesc = {};
        deviceDesc.nextInChain = nullptr;
        std::cout << "Requesting device" << std::endl;
        user_data.adapter.RequestDevice(
            &deviceDesc,
            wgpu::CallbackMode::AllowSpontaneous,
            [&](wgpu::RequestDeviceStatus status,
                wgpu::Device dv,
                wgpu::StringView message) {
                if (status != wgpu::RequestDeviceStatus::Success) {
                    std::cerr << "Failed to get an device: " << message.data;
                    throw std::runtime_error("Failed to get an device");
                }
                std::cout << "Device is created" << std::endl;
                user_data.device = std::move(dv);
            });
        return std::make_unique<InitSurface>(user_data);
    }
};

struct InitAdapter : public App {
    UserData& user_data;

    InitAdapter() = delete;

    InitAdapter(UserData& user_data)
        : App(Context(nullptr), "Init Adapter", "", ""),
          user_data(user_data) {
    }

    AppPtr run(std::string _) override {
        std::cout << "Requesting adapter" << std::endl;
        wgpu::RequestAdapterOptions adapterOptions = {};

        adapterOptions.nextInChain = nullptr;
        adapterOptions.backendType = wgpu::BackendType::WebGPU;

        user_data.instance.RequestAdapter(
            &adapterOptions,
            wgpu::CallbackMode::AllowSpontaneous,
            [&](wgpu::RequestAdapterStatus status,
                wgpu::Adapter ad,
                wgpu::StringView message) {
                if (status != wgpu::RequestAdapterStatus::Success) {
                    std::cerr << "Failed to get an adapter: " << message.data;
                    return;
                }
                user_data.adapter = std::move(ad);
            });
        return std::make_unique<InitDevice>(user_data);
    }
};

void _iter(UserData& user_data) {
    if (g_input == nullptr) {
    } else {
        if (user_data.app == nullptr)
            user_data.app = std::make_unique<InitAdapter>(user_data);
        user_data.app = user_data.app->run(g_input->empty() ? user_data.app->defaultInput : *g_input);

        std::cout << "Running: " << user_data.app->title << std::endl;
        std::cout << user_data.app->prompt << std::endl;
        std::cout << "Default value: " << user_data.app->defaultInput << std::endl;

        g_input = nullptr;
    }
}

bool one_iter(double time, void* _userData) {
    UserData* userData = static_cast<UserData*>(_userData);

    _iter(*userData);
    /* Poll for and process events */
    glfwPollEvents();

    return true;
}

int main() {
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    int width = 640, height = 480;
    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(width, height, "Hello World", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    wgpu::InstanceDescriptor instanceDescriptor = {};
    auto instance = wgpu::CreateInstance(&instanceDescriptor);

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    /* Loop until the user closes the window */
    emscripten_request_animation_frame_loop(
        one_iter,
        new UserData(window, width, height, std::move(instance)));

    // glfwTerminate();
    return 0;
}
