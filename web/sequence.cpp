#include <emscripten/html5.h>
#include <iostream>
#include <nvrhi/nvrhi.h>
#include <GLFW/glfw3.h>
#include <webgpu/webgpu_cpp.h>
#include <emscripten/bind.h>

struct Context {
    nvrhi::DeviceHandle nvrhiDevice;

    Context() = delete;

    Context(
        nvrhi::DeviceHandle nvrhiDevice)
        : nvrhiDevice(std::move(nvrhiDevice)) {
    }
};

struct App {
    virtual ~App() = default;

    const Context& context;
    const std::string title;

    App(const Context& webGpu, std::string title)
        : context(webGpu),
          title(std::move(title)) {
    }

    virtual std::unique_ptr<App> run() =0;
};

using AppPtr = std::unique_ptr<App>;

using namespace emscripten;
static std::string g_input;

class AppRunner {
    Context ctx;
    AppPtr app;

public:
    AppRunner() = delete;

    AppRunner(Context&& ctx, AppPtr app)
        : ctx(std::move(ctx)),
          app(std::move(app)) {
    }

    void run() {
        app = app->run();
    }
};

enum Stage { Wait, Next };

struct UserData {
    GLFWwindow* window;
    wgpu::Instance* instance;
    int width;
    int height;
    wgpu::Adapter* adapter = nullptr;
    wgpu::Device* device = nullptr;
    wgpu::Queue* queue = nullptr;
    nvrhi::DeviceHandle* nvrhi_device = nullptr;
    wgpu::Surface* surface = nullptr;
    nvrhi::ShaderHandle* vertex_shader;
    nvrhi::ShaderHandle* pixel_shader;
    nvrhi::BufferHandle* random_buffer;
    nvrhi::BufferHandle* buffer;
    int count = 0;
    const void* map_ptr = nullptr;

    UserData() = delete;

    explicit UserData(GLFWwindow* window,
                      wgpu::Instance* instance,
                      int width,
                      int height)
        : window(window),
          instance(instance),
          width(width),
          height(height) {
    }
};

void _iter(UserData& user_data) {
    if (g_input == "") {
    } else {
        std::cout << "input:" << g_input << std::endl;
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
        new UserData(window, &instance, width, height));

    glfwTerminate();
    return 0;
}
