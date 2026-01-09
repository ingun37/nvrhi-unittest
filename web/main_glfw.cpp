#include <emscripten/html5.h>
#include <iostream>

#include "wgpu-util/include/wgpu-util.h"

void _iter(UserData& user_data) {
    if (user_data._stage == Stage::UNINITIALIZED) {
        user_data._stage = Stage::INITIALIZING_ADAPTER;
        request_adapter(*user_data.instance, [&](wgpu::Adapter &&adapter) {
            user_data._stage = Stage::INITIALIZED_ADAPTER;
            user_data.adapter = new wgpu::Adapter(std::move(adapter));
        });
    } else if (user_data._stage == Stage::INITIALIZED_ADAPTER) {
        user_data._stage = Stage::INITIALIZING_DEVICE;
        request_device(*user_data.adapter, [&](wgpu::Device &&device) {
            user_data._stage = Stage::INITIALIZED_DEVICE;
            user_data.device = new wgpu::Device(std::move(device));
        });
    } else if (user_data._stage == Stage::INITIALIZED_DEVICE) {
        user_data.queue = new wgpu::Queue(user_data.device->GetQueue());
        user_data.nvrhi_device = new nvrhi::DeviceHandle(
            nvrhi::webgpu::createDevice({*user_data.device, *user_data.queue}));

        user_data.surface = create_surface(user_data.instance);
        configure_surface(user_data.adapter, user_data.device, user_data.surface, user_data.width, user_data.height);
        user_data._stage = Stage::INITIALIZED_SURFACE;
    } else if (user_data._stage == Stage::INITIALIZED_SURFACE) {
        std::cout << "creating shaders" << std::endl;
        user_data.vertex_shader = create_vertex_shader(
            "/Users/ingun/CLionProjects/nvrhi-unit-test/cmake-build-webgpu/shaders/generated_wgsl/shader.wgsl",
            *user_data.nvrhi_device);
        user_data.pixel_shader = create_pixel_shader(
            "/Users/ingun/CLionProjects/nvrhi-unit-test/cmake-build-webgpu/shaders/generated_wgsl/shader.wgsl",
            *user_data.nvrhi_device);
        user_data._stage = Stage::INITIALIZED_ALL;
        std::cout << "DONE!!" << std::endl;
    } else if (user_data._stage == Stage::INITIALIZED_ALL) {
        std::cout << "Creating buffer" << std::endl;
        user_data.random_buffer = create_random_buffer(
            *user_data.nvrhi_device
        );
        user_data.buffer = create_buffer(*user_data.nvrhi_device);
        user_data._stage = Stage::BUFFER_CREATED;
    } else if (user_data._stage == Stage::BUFFER_CREATED) {
        std::cout << "Mapping buffer" << std::endl;
        user_data._stage = Stage::BUFFER_MAPPED;

        // map_buffer(*user_data.nvrhi_device,
        //            *user_data.buffer,
        //            [&](const void* data) {
        //                std::cout << "Buffer read complete" << std::endl;
        //                user_data.map_ptr = data;
        //                user_data._stage = Stage::BUFFER_MAPPED;
        //            });
    } else if (user_data._stage == Stage::BUFFER_MAPPED) {
        if (user_data.count++ > 1) user_data._stage = Stage::BUFFER_COPY_READY;
    } else if (user_data._stage == Stage::BUFFER_COPY_READY) {
        std::cout << "Copying buffer" << std::endl;
        copy_buffer(*user_data.nvrhi_device, *user_data.random_buffer, *user_data.buffer);
        user_data._stage = Stage::BUFFER_COPIED;
    } else if (user_data._stage == Stage::BUFFER_COPIED) {
        if (user_data.count++ > 2) user_data._stage = Stage::BUFFER_READ_READY;
    } else if (user_data._stage == Stage::BUFFER_READ_READY) {
        map_buffer(*user_data.nvrhi_device,
                   *user_data.buffer,
                   [&](const void* data) {
                       std::cout << "Buffer read complete" << std::endl;
                       user_data.map_ptr = data;
                       user_data._stage = Stage::EXITING;
                   });
    } else if (user_data._stage == Stage::EXITING) {
        const uint8_t* ptr = reinterpret_cast<const uint8_t*>(user_data.map_ptr);
        const int pixel_pitch = 4 * 8;
        const int offset = pixel_pitch * 1210;
        std::cout << (int)ptr[offset + 0] << ", " << (int)ptr[offset + 1] << ", " << (int)ptr[offset + 2] << ", " << (
                int)ptr[offset + 3] <<
            std::endl;

        std::cout << "Exiting" << std::endl;
        user_data._stage = Stage::EXIT;
    }
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
    emscripten_request_animation_frame_loop(
        one_iter, new UserData(window, &instance, width, height));

    glfwTerminate();
    return 0;
}
