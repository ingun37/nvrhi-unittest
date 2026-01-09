#include <emscripten/html5.h>
#include <iostream>
#include <utility>
#include <nvrhi/nvrhi.h>
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

struct InitDevice : public App {
    const wgpu::Adapter& adapter;

    InitDevice() = delete;

    InitDevice(const wgpu::Adapter& adapter)
        : App("Init Device"),
          adapter(adapter) {
    }

    void run(std::function<void(std::unique_ptr<App> app_ptr)> callback) override {
        wgpu::DeviceDescriptor deviceDesc = {};
        deviceDesc.nextInChain = nullptr;
        std::cout << "Requesting device" << std::endl;
        adapter.RequestDevice(
            &deviceDesc,
            wgpu::CallbackMode::AllowSpontaneous,
            [cb = std::move(callback)](wgpu::RequestDeviceStatus status,
                                       wgpu::Device dv,
                                       wgpu::StringView message) {
                if (status != wgpu::RequestDeviceStatus::Success) {
                    std::cerr << "Failed to get an device: " << message.data;
                    throw std::runtime_error("Failed to get an device");
                }
                std::cout << "Device is created" << std::endl;
                throw std::runtime_error("Device is created");
                // cb(std::move(dv));
            });
    }
};

struct InitApp : public App {
    wgpu::Instance instance;

    InitApp() = delete;

    InitApp(wgpu::Instance&& instance)
        : App("Init Adapter"),
          instance(instance) {
    }

    void run(std::function<void(std::unique_ptr<App> app_ptr)> callback) override {
        std::cout << "Requesting adapter" << std::endl;
        wgpu::RequestAdapterOptions adapterOptions = {};

        adapterOptions.nextInChain = nullptr;
        adapterOptions.backendType = wgpu::BackendType::WebGPU;

        instance.RequestAdapter(
            &adapterOptions,
            wgpu::CallbackMode::AllowSpontaneous,
            [cb = std::move(callback)](wgpu::RequestAdapterStatus status,
                                       wgpu::Adapter adapter,
                                       wgpu::StringView message) {
                if (status != wgpu::RequestAdapterStatus::Success) {
                    std::cerr << "Failed to get an adapter: " << message.data;
                    return;
                } else {
                    std::cout << "Adapter is created" << std::endl;
                    cb(std::make_unique<InitDevice>(std::move(adapter)));
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
        new UserData(new AppRunner(std::make_unique<InitApp>(std::move(instance)))));

    glfwTerminate();
    return 0;
}
