#include <emscripten/emscripten.h>
#include <GLFW/glfw3.h>
#include <nvrhi/nvrhi.h>
#include <nvrhi/webgpu.h>
#include <webgpu/webgpu_cpp.h>
#include <emscripten/html5.h>
#include <emscripten/val.h>
#include <iostream>

enum Stage {
    UNINITIALIZED,
    INITIALIZING_ADAPTER,
    INITIALIZED_ADAPTER,
    INITIALIZING_DEVICE,
    INITIALIZED_DEVICE,
    INITIALIZING_NVRHI,
    INITIALIZED_NVRHI,
};

struct UserData {
    GLFWwindow *window;
    wgpu::Instance *instance;
    int width;
    int height;
    Stage _stage = Stage::UNINITIALIZED;
    wgpu::Adapter *adapter = nullptr;
    wgpu::Device *device = nullptr;
    wgpu::Queue *queue = nullptr;
    nvrhi::DeviceHandle *nvrhi_device = nullptr;
    wgpu::Surface *surface = nullptr;

    UserData() = delete;

    explicit UserData(GLFWwindow *window, wgpu::Instance *instance, int width, int height) : window(window),
        instance(instance), width(width), height(height) {
    }
};

emscripten::val _iter(UserData &user_data) {
    auto instance = user_data.instance;
    wgpu::RequestAdapterOptions adapterOptions = {};
    if (user_data._stage == Stage::UNINITIALIZED) {
        std::cout << "Requesting adapter" << std::endl;
        user_data._stage = Stage::INITIALIZING_ADAPTER;
        adapterOptions.nextInChain = nullptr;
        adapterOptions.backendType = wgpu::BackendType::WebGPU;
        instance->RequestAdapter(
            &adapterOptions,
            wgpu::CallbackMode::AllowSpontaneous,
            [&](wgpu::RequestAdapterStatus status,
                wgpu::Adapter adapter,
                wgpu::StringView message) {
                if (status != wgpu::RequestAdapterStatus::Success) {
                    std::cerr << "Failed to get an adapter: " << message.data;
                    return;
                } else {
                    std::cout << "Adapter is created" << std::endl;
                    user_data.adapter = new wgpu::Adapter(std::move(adapter));
                    user_data._stage = Stage::INITIALIZED_ADAPTER;
                }
            });
    } else if (user_data._stage == Stage::INITIALIZED_ADAPTER) {
        wgpu::DeviceDescriptor deviceDesc = {};
        deviceDesc.nextInChain = nullptr;
        std::cout << "Requesting device" << std::endl;
        user_data.adapter->RequestDevice(
            &deviceDesc,
            wgpu::CallbackMode::AllowSpontaneous,
            [&](wgpu::RequestDeviceStatus status, wgpu::Device dv, wgpu::StringView message) {
                if (status != wgpu::RequestDeviceStatus::Success) {
                    std::cerr << "Failed to get an device: " << message.data;
                    throw std::runtime_error("Failed to get an device");
                }
                std::cout << "Device is created" << std::endl;
                user_data.device = new wgpu::Device(std::move(dv));
                user_data.queue = new wgpu::Queue(user_data.device->GetQueue());
                user_data._stage = Stage::INITIALIZED_DEVICE;
            });
    } else if (user_data._stage == Stage::INITIALIZED_DEVICE) {
        user_data.nvrhi_device = new nvrhi::DeviceHandle(
            nvrhi::webgpu::createDevice({*user_data.device, *user_data.queue}));

        wgpu::EmscriptenSurfaceSourceCanvasHTMLSelector fromCanvasHTMLSelector;
        fromCanvasHTMLSelector.nextInChain = nullptr;
        fromCanvasHTMLSelector.sType = wgpu::SType::EmscriptenSurfaceSourceCanvasHTMLSelector;
        fromCanvasHTMLSelector.selector.data = "#canvas";
        fromCanvasHTMLSelector.selector.length = 7;

        wgpu::SurfaceDescriptor surfaceDescriptor;
        surfaceDescriptor.nextInChain = &fromCanvasHTMLSelector;
        surfaceDescriptor.label.data = "canvas_surface";
        surfaceDescriptor.label.length = 14;
        user_data.surface = new wgpu::Surface(user_data.instance->CreateSurface(&surfaceDescriptor));

        wgpu::SurfaceCapabilities capabilities;
        user_data.surface->GetCapabilities(*user_data.adapter, &capabilities);
        wgpu::SurfaceConfiguration config = {};
        config.device = *user_data.device;
        config.format = capabilities.formats[0];
        config.width = user_data.width;
        config.height = user_data.height;
        config.presentMode = capabilities.presentModes[0];
        user_data.surface->Configure(&config);

        std::cout << "Surface is created" << std::endl;
        user_data._stage = Stage::INITIALIZED_NVRHI;
    }
    return emscripten::val::undefined();
}

bool one_iter(double time, void *_userData) {
    UserData *userData = static_cast<UserData *>(_userData);

    _iter(*userData);
    /* Poll for and process events */
    glfwPollEvents();

    return true;
}

int main() {
    GLFWwindow *window;

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
    emscripten_request_animation_frame_loop(one_iter, new UserData(window, &instance, width, height));


    glfwTerminate();
    return 0;
}
