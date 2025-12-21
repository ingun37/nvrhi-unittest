//
// Created by Ingun Jon on 12/21/25.
//
#include "webgpu-glfw-util.h"

wgpu::Surface create_webgpu_surface(GLFWwindow* window,
                                    wgpu::Instance& instance,
                                    wgpu::Adapter& adapter,
                                    wgpu::Device& device,
                                    int width,
                                    int height) {
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
    return surface;
}


