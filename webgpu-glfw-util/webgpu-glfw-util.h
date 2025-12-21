//
// Created by Ingun Jon on 12/21/25.
//

#ifndef NVRHI_UNIT_TEST_WEBGPU__GLFW_UTIL_H
#define NVRHI_UNIT_TEST_WEBGPU__GLFW_UTIL_H

#include <webgpu/webgpu_cpp.h>
#include <GLFW/glfw3.h>


std::unique_ptr<wgpu::ChainedStruct, void (*)(wgpu::ChainedStruct*)> SetupWindowAndGetSurfaceDescriptorCocoa(
    GLFWwindow* window);

wgpu::Surface create_webgpu_surface(GLFWwindow* window,
                                    wgpu::Instance& instance,
                                    wgpu::Adapter& adapter,
                                    wgpu::Device& device,
                                    int width,
                                    int height);

#endif //NVRHI_UNIT_TEST_WEBGPU__GLFW_UTIL_H