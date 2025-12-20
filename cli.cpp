//
// Created by Ingun Jon on 12/20/25.
//

#include <iostream>
#include <string>
#include <utility>
#include <webgpu/webgpu_cpp.h>
#include <nvrhi/nvrhi.h>
#include <nvrhi/webgpu.h>
#include "util.h"
#include "helper.h"
#include <memory>

struct WebGPU {
    wgpu::Instance instance;
    wgpu::Adapter adapter;
    wgpu::Device device;
    wgpu::Queue queue;
    nvrhi::DeviceHandle nvrhiDevice;

    WebGPU() = delete;

    WebGPU(wgpu::Instance instance,
           wgpu::Adapter adapter,
           wgpu::Device device,
           wgpu::Queue queue,
           nvrhi::DeviceHandle nvrhiDevice)
        : instance(std::move(instance)),
          adapter(std::move(adapter)),
          device(std::move(device)),
          queue(std::move(queue)),
          nvrhiDevice(std::move(nvrhiDevice)) {
    }
};

struct App {
    virtual ~App() = default;

    const WebGPU& webGpu;
    const std::string title;

    App(const WebGPU& webGpu, std::string title)
        : webGpu(webGpu),
          title(std::move(title)) {
    }

    virtual std::unique_ptr<App> run() =0;
};

using AppPtr = std::unique_ptr<App>;

struct CommandExecution : public App {
    nvrhi::CommandListHandle commandList;
    nvrhi::TextureHandle destTexture;
    nvrhi::TextureSlice destSlice;
    nvrhi::StagingTextureHandle stagingTexture;
    nvrhi::TextureSlice stagingTextureSlice;

    CommandExecution() = delete;

    CommandExecution(const WebGPU& webGPU,
                     nvrhi::CommandListHandle commandList,
                     nvrhi::TextureHandle destTexture,
                     const nvrhi::TextureSlice& destSlice,
                     nvrhi::StagingTextureHandle stagingTexture,
                     const nvrhi::TextureSlice& stagingTextureSlice)
        : App(webGPU, "Execute command to copy staged buffer to texture"),
          commandList(std::move(commandList)),
          destTexture(std::move(destTexture)),
          destSlice(destSlice),
          stagingTexture(std::move(stagingTexture)),
          stagingTextureSlice(stagingTextureSlice) {
    }

    AppPtr run() override {
        commandList->open();
        commandList->copyTexture(destTexture, destSlice, stagingTexture, stagingTextureSlice);
        commandList->close();
        webGpu.nvrhiDevice->executeCommandList(commandList);
        return std::make_unique<CommandExecution>(webGpu,
                                                  commandList,
                                                  destTexture,
                                                  destSlice,
                                                  stagingTexture,
                                                  stagingTextureSlice);
    }
};

struct ResourceSetup : public App {
    Image image;

    ResourceSetup(Image image, const WebGPU& webGPU)
        : App(webGPU, "Set up textuere reosurces and copy image to staged buffer"),
          image(std::move(image)) {
    }

    AppPtr run() override {
        nvrhi::TextureDesc stagingTextureDesc{};
        stagingTextureDesc.width = image.width;
        stagingTextureDesc.height = image.height;
        stagingTextureDesc.format = image.format();
        auto stagingTexture = webGpu.nvrhiDevice->createStagingTexture(stagingTextureDesc, nvrhi::CpuAccessMode::Write);
        nvrhi::TextureSlice stagingTextureSlice{};
        stagingTextureSlice.width = image.width;
        stagingTextureSlice.height = image.height;
        size_t pitch;
        auto mapPtr = webGpu.nvrhiDevice->mapStagingTexture(stagingTexture,
                                                            stagingTextureSlice,
                                                            nvrhi::CpuAccessMode::Write,
                                                            &pitch);
        memcpy(mapPtr, image.data.data(), image.data.size());

        webGpu.nvrhiDevice->unmapStagingTexture(stagingTexture);

        nvrhi::TextureDesc destTextureDesc{};
        destTextureDesc.width = image.width;
        destTextureDesc.height = image.height;
        destTextureDesc.format = image.format();
        auto destTexture = webGpu.nvrhiDevice->createTexture(destTextureDesc);

        auto commandList = webGpu.nvrhiDevice->createCommandList();
        nvrhi::TextureSlice destSlice{};
        destSlice.width = image.width;
        destSlice.height = image.height;

        return std::make_unique<CommandExecution>(webGpu,
                                                  commandList,
                                                  destTexture,
                                                  destSlice,
                                                  stagingTexture,
                                                  stagingTextureSlice);
    }
};

;

struct ImageLoading : public App {
    ImageLoading() = delete;

    ImageLoading(const WebGPU& webGpu)
        : App(webGpu, "Load image from file") {
    }

    AppPtr run() override {
        return std::make_unique<ResourceSetup>(
            Image::load("/Users/ingun/CLionProjects/nvrhi-unit-test/uv_grid_opengl.png"),
            webGpu);
    }
};


int main() {
    std::string input;

    wgpu::InstanceDescriptor instanceDescriptor = nvrhi::webgpu::utils::create_instance_descriptor();
    wgpu::Instance instance = wgpu::CreateInstance(&instanceDescriptor);
    wgpu::Adapter adapter = nvrhi::webgpu::utils::create_adapter(
        instance,
        nvrhi::webgpu::utils::create_adapter_option(wgpu::BackendType::Metal, wgpu::AdapterType::IntegratedGPU));
    wgpu::Device device = create_device(instance, adapter);
    wgpu::Queue queue = device.GetQueue();
    nvrhi::DeviceHandle nvrhiDevice = nvrhi::webgpu::createDevice({device, queue});;

    WebGPU webGpu(instance, adapter, device, queue, nvrhiDevice);

    AppPtr app = std::make_unique<ImageLoading>(webGpu);
    while (true) {
        std::cout << "Enter to:" << app->title << std::endl;
        std::getline(std::cin, input);
        if (input == "exit") break;
        app = std::move(app->run());
    }
    return 0;
}