//
// Created by Ingun Jon on 12/20/25.
//

#include <nvrhi/nvrhi.h>
#include <nvrhi/webgpu.h>
#include <webgpu/webgpu_cpp.h>
#include <webgpu/webgpu_cpp_print.h>

#include <cstdlib>
#include <iostream>
#include "util.h"
#include <GLFW/glfw3.h>
#include "metal.h"
#include "../../../../Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/include/c++/v1/cstring"

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
    auto nvrhiDevice = nvrhi::webgpu::createDevice({device, queue});

    auto chainedDescriptor = SetupWindowAndGetSurfaceDescriptorCocoa(window);

    wgpu::SurfaceDescriptor descriptor;
    descriptor.nextInChain = chainedDescriptor.get();
    wgpu::Surface surface = instance.CreateSurface(&descriptor);
    wgpu::SurfaceCapabilities capabilities;
    surface.GetCapabilities(adapter, &capabilities);
    wgpu::SurfaceConfiguration config = {};
    config.device = device;
    config.format = capabilities.formats[0];
    config.width = width;
    config.height = height;
    if (capabilities.presentModeCount == 0) throw std::runtime_error("present mode count is 0");
    config.presentMode = capabilities.presentModes[0];
    surface.Configure(&config);
    auto preferredSurfaceTextureFormat = capabilities.formats[0];
    nvrhi::TextureDesc stagingTextureDesc{};
    stagingTextureDesc.format = nvrhi::Format::RGBA8_UNORM;
    auto stagingTexture = nvrhiDevice->createStagingTexture(stagingTextureDesc, nvrhi::CpuAccessMode::Write);
    nvrhi::TextureSlice stagingTextureSlice{};
    size_t pitch;
    auto mapPtr = nvrhiDevice->mapStagingTexture(stagingTexture,
                                                 stagingTextureSlice,
                                                 nvrhi::CpuAccessMode::Write,
                                                 &pitch);
    const char pixelBuff[] = {0x12, 0x34, 0x56, 0x78};
    memcpy(mapPtr, pixelBuff, 4);

    nvrhiDevice->unmapStagingTexture(stagingTexture);

    nvrhi::TextureDesc destTextureDesc{};
    destTextureDesc.format = nvrhi::Format::RGBA8_UNORM;
    auto destTexture = nvrhiDevice->createTexture(destTextureDesc);

    auto commandList = nvrhiDevice->createCommandList();
    commandList->open();
    nvrhi::TextureSlice destSlice{};
    commandList->copyTexture(destTexture, destSlice, stagingTexture, stagingTextureSlice);
    commandList->close();
    nvrhiDevice->executeCommandList(commandList);
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