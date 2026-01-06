//
// Created by Ingun Jon on 12/21/25.
//

#ifndef NVRHI_UNIT_TEST_WEBGPU_NVRHI_UTIL_H
#define NVRHI_UNIT_TEST_WEBGPU_NVRHI_UTIL_H

#include <webgpu/webgpu_cpp.h>
#include <nvrhi/nvrhi.h>

nvrhi::TextureHandle CreateBackBuffer(const nvrhi::DeviceHandle& m_NvrhiDevice, wgpu::Surface m_Surface);

#endif //NVRHI_UNIT_TEST_WEBGPU_NVRHI_UTIL_H