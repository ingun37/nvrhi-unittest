//
// Created by Ingun Jon on 12/20/25.
//

#include <nvrhi/nvrhi.h>
#include <nvrhi/webgpu.h>
#include <webgpu/webgpu_cpp.h>
#include <iostream>
#include <GLFW/glfw3.h>
#include "webgpu-util.h"
#include "webgpu-glfw-util.h"
#include "../webgpu-glfw-util/webgpu-glfw-util.h"

int main() {
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        throw std::runtime_error("Failed to initialize GLFW");
    int width = 1024, height = 1024;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(width, height, "Hello World", NULL, NULL);
    if (!window) {
        glfwTerminate();
        throw std::runtime_error("Failed to open GLFW window");
    }
    wgpu::InstanceDescriptor instanceDescriptor = nvrhi::webgpu::utils::create_instance_descriptor();
    wgpu::Instance instance = wgpu::CreateInstance(&instanceDescriptor);
    wgpu::Adapter adapter = nvrhi::webgpu::utils::create_adapter(
        instance,
        nvrhi::webgpu::utils::create_adapter_option(wgpu::BackendType::Metal, wgpu::AdapterType::IntegratedGPU));
    wgpu::Device device = create_device(instance, adapter);
    wgpu::Queue queue = device.GetQueue();
    nvrhi::DeviceHandle nvrhiDevice = nvrhi::webgpu::createDevice({device, queue});
    wgpu::Surface surface = create_webgpu_surface(window, instance, adapter, device, width, height);
    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window)) {
        /* Render here */

        // glClear(GL_COLOR_BUFFER_BIT);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
}