//
// Created by Ingun Jon on 12/24/25.
//

#include "nvrhi_util.h"

namespace nvrhi_util {
nvrhi::StagingTextureHandle create3DStaging(uint32_t width,
                                            uint32_t height,
                                            uint32_t depth,
                                            const nvrhi::Format format,
                                            const nvrhi::DeviceHandle& device) {
    nvrhi::TextureDesc stagingTextureDesc{};
    stagingTextureDesc.width = width;
    stagingTextureDesc.height = height;
    stagingTextureDesc.depth = depth;
    stagingTextureDesc.format = format;
    stagingTextureDesc.dimension = nvrhi::TextureDimension::Texture3D;
    return device->createStagingTexture(stagingTextureDesc, nvrhi::CpuAccessMode::Write);
}
}