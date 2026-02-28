//
// Created by Ingun Jon on 2/3/26.
//

#ifndef NVRHI_UNIT_TEST_BACKEND_H
#define NVRHI_UNIT_TEST_BACKEND_H
#include <string>
#include <vector>

inline std::string extension() {
#if defined(SCENARIO_VULKAN)
    return ".sprv";
#elif defined(SCENARIO_WGPU)
    return ".wgsl";
#else
    throw std::runtime_error("Unknown platform");
#endif
}

std::vector<char> read_shader(const std::string& name);
#endif //NVRHI_UNIT_TEST_BACKEND_H