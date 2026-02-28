//
// Created by Ingun Jon on 2/28/26.
//

#include "backend.h"
#include <fstream>

std::vector<char> read_shader(const std::string& name) {
    std::string shader_dir = SCENARIO_SHADERS_OUTPUT_DIR;
    std::string path = shader_dir + "/" + name + extension();
    std::ifstream file(path, std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open shader file at " + path);
    }
    // Determine file size
    file.seekg(0, std::ios::end);
    std::streampos fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // Read data into vector
    std::vector<char> fileData(fileSize);
    file.read(fileData.data(), fileSize);
    return fileData;
}