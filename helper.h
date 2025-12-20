//
// Created by Ingun Jon on 12/19/25.
//

#ifndef NVRHI_UNIT_TEST_HELPER_H
#define NVRHI_UNIT_TEST_HELPER_H
#include <nvrhi/nvrhi.h>
#include <webgpu/webgpu_cpp.h>

nvrhi::TextureHandle CreateBackBuffer(const nvrhi::DeviceHandle& m_NvrhiDevice, wgpu::Surface m_Surface);


#endif //NVRHI_UNIT_TEST_HELPER_H