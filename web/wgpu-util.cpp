//
// Created by Ingun Jon on 1/6/26.
//
#include "wgpu-util.h"

#include <iostream>

void request_adapter(const wgpu::Instance &instance, std::function<void(wgpu::Adapter &&)> callback) {
    std::cout << "Requesting adapter" << std::endl;
    wgpu::RequestAdapterOptions adapterOptions = {};

    adapterOptions.nextInChain = nullptr;
    adapterOptions.backendType = wgpu::BackendType::WebGPU;

    instance.RequestAdapter(
        &adapterOptions, wgpu::CallbackMode::AllowSpontaneous,
        [cb = std::move(callback)](wgpu::RequestAdapterStatus status, wgpu::Adapter adapter,
                                   wgpu::StringView message) {
            if (status != wgpu::RequestAdapterStatus::Success) {
                std::cerr << "Failed to get an adapter: " << message.data;
                return;
            } else {
                std::cout << "Adapter is created" << std::endl;
                cb(std::move(adapter));
            }
        });
}

void request_device(const wgpu::Adapter &adapter, std::function<void(wgpu::Device &&)> callback) {
    wgpu::DeviceDescriptor deviceDesc = {};
    deviceDesc.nextInChain = nullptr;
    std::cout << "Requesting device" << std::endl;
    adapter.RequestDevice(
        &deviceDesc, wgpu::CallbackMode::AllowSpontaneous,
        [cb = std::move(callback)](wgpu::RequestDeviceStatus status, wgpu::Device dv,
                                   wgpu::StringView message) {
            if (status != wgpu::RequestDeviceStatus::Success) {
                std::cerr << "Failed to get an device: " << message.data;
                throw std::runtime_error("Failed to get an device");
            }
            std::cout << "Device is created" << std::endl;
            cb(std::move(dv));
        });
}

wgpu::Surface *create_surface(wgpu::Instance *instance) {
    std::cout << "Creating surface" << std::endl;

    wgpu::EmscriptenSurfaceSourceCanvasHTMLSelector fromCanvasHTMLSelector;
    fromCanvasHTMLSelector.nextInChain = nullptr;
    fromCanvasHTMLSelector.sType =
            wgpu::SType::EmscriptenSurfaceSourceCanvasHTMLSelector;
    fromCanvasHTMLSelector.selector.data = "#canvas";
    fromCanvasHTMLSelector.selector.length = 7;

    wgpu::SurfaceDescriptor surfaceDescriptor;
    surfaceDescriptor.nextInChain = &fromCanvasHTMLSelector;
    surfaceDescriptor.label.data = "canvas_surface";
    surfaceDescriptor.label.length = 14;
    return new wgpu::Surface(instance->CreateSurface(&surfaceDescriptor));
}

void configure_surface(
    wgpu::Adapter *adapter,
    wgpu::Device *device,
    wgpu::Surface *surface,
    int width, int height
) {
    std::cout << "Configuring surface" << std::endl;
    wgpu::SurfaceCapabilities capabilities;
    surface->GetCapabilities(*adapter, &capabilities);
    wgpu::SurfaceConfiguration config = {};
    config.device = *device;
    config.format = capabilities.formats[0];
    config.width = width;
    config.height = height;
    config.presentMode = capabilities.presentModes[0];
    surface->Configure(&config);
}
