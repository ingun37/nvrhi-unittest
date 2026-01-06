//
// Created by Ingun Jon on 1/6/26.
//
#include "wgpu-util.h"
#include <nvrhi/webgpu.h>

#include <iostream>
void request_adapter(UserData &user_data) {

  std::cout << "Requesting adapter" << std::endl;
  wgpu::RequestAdapterOptions adapterOptions = {};

  user_data._stage = Stage::INITIALIZING_ADAPTER;
  adapterOptions.nextInChain = nullptr;
  adapterOptions.backendType = wgpu::BackendType::WebGPU;
  user_data.instance->RequestAdapter(
      &adapterOptions, wgpu::CallbackMode::AllowSpontaneous,
      [&](wgpu::RequestAdapterStatus status, wgpu::Adapter adapter,
          wgpu::StringView message) {
        if (status != wgpu::RequestAdapterStatus::Success) {
          std::cerr << "Failed to get an adapter: " << message.data;
          return;
        } else {
          std::cout << "Adapter is created" << std::endl;
          user_data.adapter = new wgpu::Adapter(std::move(adapter));
          user_data._stage = Stage::INITIALIZED_ADAPTER;
        }
      });
}

void request_device(UserData &user_data) {
  wgpu::DeviceDescriptor deviceDesc = {};
  deviceDesc.nextInChain = nullptr;
  std::cout << "Requesting device" << std::endl;
  user_data.adapter->RequestDevice(
      &deviceDesc, wgpu::CallbackMode::AllowSpontaneous,
      [&](wgpu::RequestDeviceStatus status, wgpu::Device dv,
          wgpu::StringView message) {
        if (status != wgpu::RequestDeviceStatus::Success) {
          std::cerr << "Failed to get an device: " << message.data;
          throw std::runtime_error("Failed to get an device");
        }
        std::cout << "Device is created" << std::endl;
        user_data.device = new wgpu::Device(std::move(dv));
        user_data.queue = new wgpu::Queue(user_data.device->GetQueue());
        user_data._stage = Stage::INITIALIZED_DEVICE;
      });
}

void create_surface(UserData &user_data) {
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
  user_data._stage = Stage::INITIALIZED_NVRHI;
}