//
// Created by Ingun Jon on 1/6/26.
//

#ifndef ASYNC_TEST_WGPU_UTIL_H
#define ASYNC_TEST_WGPU_UTIL_H
#include <GLFW/glfw3.h>
#include <nvrhi/nvrhi.h>
#include <nvrhi/webgpu.h>
#include <webgpu/webgpu_cpp.h>
#include <functional>
#include "my_math.h"

namespace dm = donut::math;
namespace math = donut::math;

enum Stage {
    UNINITIALIZED,
    INITIALIZING_ADAPTER,
    INITIALIZED_ADAPTER,
    INITIALIZING_DEVICE,
    INITIALIZED_DEVICE,
    INITIALIZED_SURFACE,
    INITIALIZED_ALL,
    STAGING_CREATED,
    EXITING,
    EXIT
};

struct Vertex {
    math::float3 position;
    math::float2 uv;
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
    nvrhi::ShaderHandle *vertex_shader;
    nvrhi::ShaderHandle *pixel_shader;
    nvrhi::StagingTextureHandle *staging;
    nvrhi::BufferHandle* buffer;

    UserData() = delete;

    explicit UserData(GLFWwindow *window, wgpu::Instance *instance, int width,
                      int height)
        : window(window), instance(instance), width(width), height(height) {
    }
};

void request_adapter(const wgpu::Instance &instance,
                     std::function<void(wgpu::Adapter &&)> callback);

void request_device(const wgpu::Adapter &adapter, std::function<void(wgpu::Device &&)> callback);

wgpu::Surface *create_surface(wgpu::Instance *instance);

void configure_surface(
    wgpu::Adapter *adapter,
    wgpu::Device *device,
    wgpu::Surface *surface,
    int width, int height
);

nvrhi::ShaderHandle *create_vertex_shader(const std::string &shaderFilePath, nvrhi::DeviceHandle &nvrhi_device);

nvrhi::ShaderHandle *create_pixel_shader(const std::string &shaderFilePath, nvrhi::DeviceHandle &nvrhi_device);

nvrhi::StagingTextureHandle *create_staging(const std::string &image_path, nvrhi::DeviceHandle &device);

nvrhi::BufferHandle* create_buffer(nvrhi::DeviceHandle& device);
void read_buffer(nvrhi::DeviceHandle& device, nvrhi::BufferHandle& buffer, std::function<void(void*)> callback);

#endif // ASYNC_TEST_WGPU_UTIL_H
