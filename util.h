//
// Created by Ingun Jon on 12/19/25.
//

#ifndef NVRHI_UNIT_TEST_UTIL_H
#define NVRHI_UNIT_TEST_UTIL_H
#include <webgpu/webgpu_cpp.h>

wgpu::Device create_device(const wgpu::Instance &instance, const wgpu::Adapter &adapter);

#endif //NVRHI_UNIT_TEST_UTIL_H
