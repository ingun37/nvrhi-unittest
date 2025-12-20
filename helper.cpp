//
// Created by Ingun Jon on 12/19/25.
//

#include "helper.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

nvrhi::TextureHandle CreateBackBuffer(const nvrhi::DeviceHandle& m_NvrhiDevice, wgpu::Surface m_Surface) {
    wgpu::SurfaceTexture surfaceTexture;
    m_Surface.GetCurrentTexture(&surfaceTexture);
    nvrhi::TextureDesc desc;
    desc.width = surfaceTexture.texture.GetWidth();
    desc.height = surfaceTexture.texture.GetHeight();
    desc.format = nvrhi::Format::RGBA8_UNORM;
    desc.mipLevels = 1;
    desc.arraySize = 1;
    desc.sampleCount = 1;
    return m_NvrhiDevice->createHandleForNativeTexture(nvrhi::ObjectTypes::WGPU_Texture,
                                                       surfaceTexture.texture.MoveToCHandle(),
                                                       desc);
}

Image Image::load(const std::string& path) {
    int imgW, imgH, imgC;
    const char* img_path = "/Users/ingun/CLionProjects/nvrhi-unit-test/uv_grid_opengl.png";
    const unsigned char* img = stbi_load(img_path, &imgW, &imgH, &imgC, 4);
    if (imgC != 4) throw std::runtime_error("invalid image channel");
    std::vector<uint8_t> data(img, img + imgW * imgH * 4);
    return {imgW, imgH, imgC, std::move(data)};
}