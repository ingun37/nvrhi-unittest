//
// Created by Ingun Jon on 1/6/26.
//
#include "wgpu-util.h"
#include <nvrhi/webgpu.h>

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

void create_surface(UserData &user_data) {
    std::cout << "Creating surface" << std::endl;
    user_data.nvrhi_device = new nvrhi::DeviceHandle(
        nvrhi::webgpu::createDevice({*user_data.device, *user_data.queue}));

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
    user_data.surface =
            new wgpu::Surface(user_data.instance->CreateSurface(&surfaceDescriptor));

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
    user_data._stage = Stage::INITIALIZED_ALL;
}
