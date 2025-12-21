//
// Created by Ingun Jon on 12/19/25.
//

#include "helper.h"

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

