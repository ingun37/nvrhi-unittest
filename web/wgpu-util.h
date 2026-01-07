//
// Created by Ingun Jon on 1/6/26.
//

#ifndef ASYNC_TEST_WGPU_UTIL_H
#define ASYNC_TEST_WGPU_UTIL_H
#include <GLFW/glfw3.h>
#include <nvrhi/nvrhi.h>
#include <webgpu/webgpu_cpp.h>
#include <functional>

enum Stage {
    UNINITIALIZED,
    INITIALIZING_ADAPTER,
    INITIALIZED_ADAPTER,
    INITIALIZING_DEVICE,
    INITIALIZED_DEVICE,
    INITIALIZED_ALL,
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

    explicit UserData(GLFWwindow *window, wgpu::Instance *instance, int width,
                      int height)
        : window(window), instance(instance), width(width), height(height) {
    }
};

void request_adapter(const wgpu::Instance &instance,
                     std::function<void(wgpu::Adapter &&)> callback);

void request_device(UserData &user_data);

void create_surface(UserData &user_data);
#endif // ASYNC_TEST_WGPU_UTIL_H
