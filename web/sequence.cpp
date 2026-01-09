#include <emscripten/html5.h>
#include <iostream>
#include <utility>
#include <nvrhi/nvrhi.h>
#include <nvrhi/webgpu.h>
#include <GLFW/glfw3.h>
#include <webgpu/webgpu_cpp.h>
#include <emscripten/bind.h>

struct App {
    virtual ~App() = default;

    const std::string title;

    App(std::string title)
        : title(std::move(title)) {
    }

    virtual void run(std::function<void(std::unique_ptr<App> app_ptr)>) =0;
};

using AppPtr = std::unique_ptr<App>;


using namespace emscripten;
static std::string g_input;

class AppRunner {
    AppPtr app;

public:
    AppRunner() = delete;

    AppRunner(AppPtr app)
        : app(std::move(app)) {
    }

    void run() {
        std::cout << "Running: " << app->title << std::endl;
        app->run([this](AppPtr new_app) {
            this->app = std::move(new_app);
        });
    }
};

enum Stage { Wait, Next };

struct UserData {
    AppRunner* app_runner;

    UserData() = delete;

    explicit UserData(
        AppRunner* app_runner)
        : app_runner(app_runner) {
    }
};

void _iter(UserData& user_data) {
    if (g_input == "") {
    } else {
        std::cout << "input:" << g_input << std::endl;
        user_data.app_runner->run();
        g_input = "";
    }
}

bool one_iter(double time, void* _userData) {
    UserData* userData = static_cast<UserData*>(_userData);

    _iter(*userData);
    /* Poll for and process events */
    glfwPollEvents();

    return true;
}


void run_next_app() {
    g_input = "a";
}

EMSCRIPTEN_BINDINGS(my_module) {
    function("run_next_app", &run_next_app);
}

struct InitSurface : public App {
    wgpu::Instance instance;
    wgpu::Adapter adapter;
    wgpu::Device device;
    int width;
    int height;

    InitSurface() = delete;

    InitSurface(GLFWwindow* window,
                wgpu::Instance&& instance,
                wgpu::Adapter&& adapter,
                wgpu::Device device,
                int width,
                int height)
        : App("Init Surface"),
          instance(instance),
          adapter(adapter),
          device(std::move(device)),
          width(width),
          height(height) {
    }

    void run(std::function<void(std::unique_ptr<App> app_ptr)>) override {
        auto queue = device.GetQueue();
        auto nvrhi_device = new nvrhi::DeviceHandle(nvrhi::webgpu::createDevice({device, queue}));
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
        auto surface = instance.CreateSurface(&surfaceDescriptor);

        std::cout << "Configuring surface" << std::endl;
        wgpu::SurfaceCapabilities capabilities;
        surface.GetCapabilities(adapter, &capabilities);
        wgpu::SurfaceConfiguration config = {};
        config.device = device;
        config.format = capabilities.formats[0];
        config.width = width;
        config.height = height;
        config.presentMode = capabilities.presentModes[0];
        surface.Configure(&config);
        throw new std::runtime_error("not implemented");
    }
};

struct InitDevice : public App {
    GLFWwindow* window;
    wgpu::Instance instance;
    wgpu::Adapter adapter;
    int width;
    int height;

    InitDevice() = delete;

    InitDevice(GLFWwindow* window,
               wgpu::Instance&& instance,
               wgpu::Adapter&& adapter,
               int width,
               int height)
        : App("Init Device"),
          window(window),
          instance(instance),
          adapter(adapter),
          width(width),
          height(height) {
    }

    void run(std::function<void(std::unique_ptr<App> app_ptr)> callback) override {
        wgpu::DeviceDescriptor deviceDesc = {};
        deviceDesc.nextInChain = nullptr;
        std::cout << "Requesting device" << std::endl;
        adapter.RequestDevice(
            &deviceDesc,
            wgpu::CallbackMode::AllowSpontaneous,
            [this,cb = std::move(callback)](wgpu::RequestDeviceStatus status,
                                            wgpu::Device dv,
                                            wgpu::StringView message) {
                if (status != wgpu::RequestDeviceStatus::Success) {
                    std::cerr << "Failed to get an device: " << message.data;
                    throw std::runtime_error("Failed to get an device");
                }
                std::cout << "Device is created" << std::endl;
                cb(std::make_unique<InitSurface>(window,
                                                 std::move(instance),
                                                 std::move(adapter),
                                                 std::move(dv),
                                                 width,
                                                 height));
            });
    }
};

struct InitAdapter : public App {
    GLFWwindow* window;
    wgpu::Instance instance;
    int width;
    int height;

    InitAdapter() = delete;

    InitAdapter(GLFWwindow* window, wgpu::Instance&& instance, int width, int height)
        : App("Init Adapter"),
          window(window),
          instance(instance),
          width(width),
          height(height) {
    }

    void run(std::function<void(std::unique_ptr<App> app_ptr)> callback) override {
        std::cout << "Requesting adapter" << std::endl;
        wgpu::RequestAdapterOptions adapterOptions = {};

        adapterOptions.nextInChain = nullptr;
        adapterOptions.backendType = wgpu::BackendType::WebGPU;

        instance.RequestAdapter(
            &adapterOptions,
            wgpu::CallbackMode::AllowSpontaneous,
            [this,cb = std::move(callback)](wgpu::RequestAdapterStatus status,
                                            wgpu::Adapter adapter,
                                            wgpu::StringView message) {
                if (status != wgpu::RequestAdapterStatus::Success) {
                    std::cerr << "Failed to get an adapter: " << message.data;
                    return;
                } else {
                    std::cout << "Adapter is created" << std::endl;
                    cb(std::make_unique<InitDevice>(window, std::move(instance), std::move(adapter), width, height));
                }
            });
    }
};

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
    wgpu::Instance instance = wgpu::CreateInstance(&instanceDescriptor);

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    /* Loop until the user closes the window */
    emscripten_request_animation_frame_loop(
        one_iter,
        new UserData(new AppRunner(std::make_unique<InitAdapter>(window, std::move(instance), width, height))));

    // glfwTerminate();
    return 0;
}
