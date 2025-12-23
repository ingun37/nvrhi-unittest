//
// Created by Ingun Jon on 12/21/25.
//

#ifndef NVRHI_UNIT_TEST_APP_H
#define NVRHI_UNIT_TEST_APP_H
#include <iostream>
#include <string>
#include <utility>
#include <nvrhi/nvrhi.h>
#include <memory>
#include <sstream>
#include <Image.h>

nvrhi::Format format(const Image& img) {
    if (img.channel == 4) return nvrhi::Format::RGBA8_UNORM;
    throw std::runtime_error("invalid image channel");
}

struct Context {
    nvrhi::DeviceHandle nvrhiDevice;

    Context() = delete;

    Context(
        nvrhi::DeviceHandle nvrhiDevice)
        : nvrhiDevice(std::move(nvrhiDevice)) {
    }
};

struct App {
    virtual ~App() = default;

    const Context& context;
    const std::string title;

    App(const Context& webGpu, std::string title)
        : context(webGpu),
          title(std::move(title)) {
    }

    virtual std::unique_ptr<App> run() =0;
};

using AppPtr = std::unique_ptr<App>;

struct CommandExecution : public App {
    nvrhi::CommandListHandle commandList;
    nvrhi::TextureHandle destTexture;
    nvrhi::StagingTextureHandle stagingTexture;

    CommandExecution() = delete;

    CommandExecution(const Context& webGPU,
                     nvrhi::CommandListHandle commandList,
                     nvrhi::TextureHandle destTexture,
                     nvrhi::StagingTextureHandle stagingTexture)
        : App(webGPU, "Execute command to copy staged buffer to texture"),
          commandList(std::move(commandList)),
          destTexture(std::move(destTexture)),
          stagingTexture(std::move(stagingTexture)) {
    }

    AppPtr run() override {
        std::cout << "Enter width_ratio and height_ratio (space-separated, default: 0.5 0.5): ";
        std::string input;
        std::getline(std::cin, input);

        float width_ratio = 0.5f;
        float height_ratio = 0.5f;

        if (!input.empty()) {
            std::istringstream iss(input);
            float temp_width, temp_height;
            if (iss >> temp_width >> temp_height) {
                width_ratio = temp_width;
                height_ratio = temp_height;
            }
        }

        uint32_t original_width = stagingTexture->getDesc().width;
        uint32_t original_height = stagingTexture->getDesc().height;
        uint32_t width = static_cast<uint32_t>(original_width * width_ratio);
        uint32_t height = static_cast<uint32_t>(original_height * height_ratio);
        nvrhi::TextureSlice destSlice{};
        destSlice.x = 0;
        destSlice.width = width;
        destSlice.height = height;
        nvrhi::TextureSlice srcTextureSlice{};
        srcTextureSlice.width = original_width;
        srcTextureSlice.height = original_height;

        commandList->open();
        commandList->copyTexture(destTexture, destSlice, stagingTexture, srcTextureSlice);
        commandList->close();
        context.nvrhiDevice->executeCommandList(commandList);
        return std::make_unique<CommandExecution>(context,
                                                  commandList,
                                                  destTexture,
                                                  stagingTexture
            );
    }
};

struct ResourceSetup : public App {
    Image image;
    uint32_t dest_x;

    ResourceSetup(Image image, const Context& webGPU, uint32_t dest_x)
        : App(webGPU, "Set up textuere reosurces and copy image to staged buffer"),
          image(std::move(image)),
          dest_x(dest_x) {
    }

    AppPtr run() override {
        nvrhi::TextureDesc stagingTextureDesc{};
        stagingTextureDesc.width = image.width;
        stagingTextureDesc.height = image.height;
        stagingTextureDesc.format = format(image);
        auto stagingTexture = context.nvrhiDevice->
            createStagingTexture(stagingTextureDesc, nvrhi::CpuAccessMode::Write);
        nvrhi::TextureSlice stagingTextureSlice{};
        stagingTextureSlice.width = image.width;
        stagingTextureSlice.height = image.height;
        size_t pitch;
        const uint32_t actualRowSize = image.data.size() / image.height;
        auto mapPtr = static_cast<uint8_t*>(context.nvrhiDevice->mapStagingTexture(stagingTexture,
            stagingTextureSlice,
            nvrhi::CpuAccessMode::Write,
            &pitch));
        for (int i = 0; i < image.height; ++i) {
            memcpy(mapPtr + (i * pitch),
                   image.data.data() + (i * actualRowSize),
                   actualRowSize);
        }

        context.nvrhiDevice->unmapStagingTexture(stagingTexture);
        nvrhi::TextureDesc destTextureDesc{};
        destTextureDesc.width = image.width * 2;
        destTextureDesc.height = image.height * 2;
        destTextureDesc.format = format(image);
        auto destTexture = context.nvrhiDevice->createTexture(destTextureDesc);

        auto commandList = context.nvrhiDevice->createCommandList();

        return std::make_unique<CommandExecution>(context,
                                                  commandList,
                                                  destTexture,
                                                  stagingTexture
            );
    }
};

struct ImageLoading : public App {
    ImageLoading() = delete;

    ImageLoading(const Context& webGpu)
        : App(webGpu, "Load image from file") {
    }

    AppPtr run() override {
        return std::make_unique<ResourceSetup>(
            Image::load("/Users/ingun/CLionProjects/nvrhi-unit-test/uv_grid_opengl_small_remainder.png"),
            context,
            256);
    }
};

void run_app(Context&& webGpu) {
    std::string input;

    AppPtr app = std::make_unique<ImageLoading>(webGpu);
    while (true) {
        std::cout << "Enter to:" << app->title << std::endl;
        std::getline(std::cin, input);
        if (input == "exit") break;
        app = std::move(app->run());
    }
}

#endif //NVRHI_UNIT_TEST_APP_H