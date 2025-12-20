//
// Created by Ingun Jon on 12/19/25.
//

#ifndef NVRHI_UNIT_TEST_HELPER_H
#define NVRHI_UNIT_TEST_HELPER_H
#include <nvrhi/nvrhi.h>
#include <webgpu/webgpu_cpp.h>

#include <utility>

nvrhi::TextureHandle CreateBackBuffer(const nvrhi::DeviceHandle& m_NvrhiDevice, wgpu::Surface m_Surface);

struct Image {
    Image() = delete;

    int width;
    int height;
    int channel;
    std::vector<uint8_t> data;

    Image(int width, int height, int channel, std::vector<uint8_t> data)
        : width(width),
          height(height),
          channel(channel),
          data(std::move(data)) {
    }

    static Image load(const std::string& path);

    [[nodiscard]] nvrhi::Format format() const {
        if (channel == 4) return nvrhi::Format::RGBA8_UNORM;
        throw std::runtime_error("invalid image channel");
    }
};


#endif //NVRHI_UNIT_TEST_HELPER_H