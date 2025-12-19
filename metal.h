//
// Created by Ingun Jon on 12/19/25.
//

#ifndef NVRHI_UNIT_TEST_METAL_H
#define NVRHI_UNIT_TEST_METAL_H
#include <memory>
#include "GLFW/glfw3.h"

#include <webgpu/webgpu_cpp.h>

// SetupWindowAndGetSurfaceDescriptorCocoa defined in GLFWUtils_metal.mm
std::unique_ptr<wgpu::ChainedStruct, void (*)(wgpu::ChainedStruct*)>
SetupWindowAndGetSurfaceDescriptorCocoa(GLFWwindow* window);

#endif //NVRHI_UNIT_TEST_METAL_H
