#include <emscripten/html5.h>
#include <emscripten/val.h>
#include <iostream>

#include "wgpu-util.h"

emscripten::val _iter(UserData &user_data) {
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
        create_surface(user_data);
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
    emscripten_request_animation_frame_loop(
        one_iter, new UserData(window, &instance, width, height));

    glfwTerminate();
    return 0;
}
