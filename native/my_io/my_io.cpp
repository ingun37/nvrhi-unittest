//
// Created by Ingun Jon on 12/24/25.
//

#include "my_io.h"
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

namespace my_io {
std::string prompt_rect(uint32_t default_x, uint32_t default_y, uint32_t default_width, uint32_t default_height) {
    return "Enter x, y, width, height (comma separated).\n"
           "Defaults: " + std::to_string(default_x) + ", " + std::to_string(default_y) + ", " +
           std::to_string(default_width) + ", " + std::to_string(default_height) + "\n"
           "Skip an input by leaving it empty (e.g., ',,100,').\n> ";
}


Rect parse_rect(const std::string& input) {
    Rect result = {0, 0, 0, 0};
    if (input.empty()) return result;

    std::stringstream ss(input);
    std::string segment;
    uint32_t* targets[] = {&result.x, &result.y, &result.width, &result.height};

    for (int i = 0; i < 4; ++i) {
        if (std::getline(ss, segment, ',')) {
            segment.erase(0, segment.find_first_not_of(" \t"));
            size_t last = segment.find_last_not_of(" \t");
            if (last != std::string::npos) segment.erase(last + 1);

            if (!segment.empty()) {
                try {
                    *targets[i] = static_cast<uint32_t>(std::stoul(segment));
                } catch (...) {
                    *targets[i] = 0;
                }
            } else {
                *targets[i] = 0;
            }
        } else {
            *targets[i] = 0;
        }
    }
    return result;
}

std::string default_input_rect(uint32_t default_x,
                               uint32_t default_y,
                               uint32_t default_width,
                               uint32_t default_height) {
    return std::to_string(default_x) + "," + std::to_string(default_y) + "," +
           std::to_string(default_width) + "," + std::to_string(default_height);
}

std::string prompt_box(uint32_t default_x,
                       uint32_t default_y,
                       uint32_t default_z,
                       uint32_t default_width,
                       uint32_t default_height,
                       uint32_t default_depth) {
    return "Enter x, y, z, width, height, depth (comma separated).\n"
           "Defaults: " + std::to_string(default_x) + ", " + std::to_string(default_y) + ", " +
           std::to_string(default_z) + ", " + std::to_string(default_width) + ", " +
           std::to_string(default_height) + ", " + std::to_string(default_depth) + "\n"
           "Skip an input by leaving it empty (e.g., ',,,,100,').\n> ";
}


Box parse_box(const std::string& input) {
    Box result = {0, 0, 0, 0, 0, 0};
    if (input.empty()) return result;

    std::stringstream ss(input);
    std::string segment;
    uint32_t* targets[] = {&result.x, &result.y, &result.z, &result.width, &result.height, &result.depth};

    for (int i = 0; i < 6; ++i) {
        if (std::getline(ss, segment, ',')) {
            segment.erase(0, segment.find_first_not_of(" \t"));
            size_t last = segment.find_last_not_of(" \t");
            if (last != std::string::npos) segment.erase(last + 1);

            if (!segment.empty()) {
                try {
                    *targets[i] = static_cast<uint32_t>(std::stoul(segment));
                } catch (...) {
                    *targets[i] = 0;
                }
            } else {
                *targets[i] = 0;
            }
        } else {
            *targets[i] = 0;
        }
    }
    return result;
}

std::string default_input_box(uint32_t default_x,
                              uint32_t default_y,
                              uint32_t default_z,
                              uint32_t default_width,
                              uint32_t default_height,
                              uint32_t default_depth) {
    return std::to_string(default_x) + "," + std::to_string(default_y) + "," + std::to_string(default_z) + "," +
           std::to_string(default_width) + "," + std::to_string(default_height) + "," + std::to_string(default_depth);
}

std::string prompt_Extent3D(uint32_t default_width, uint32_t default_height, uint32_t default_depth) {
    return "Enter width, height, depth (comma separated).\n"
           "Defaults: " + std::to_string(default_width) + ", " +
           std::to_string(default_height) + ", " + std::to_string(default_depth) + "\n"
           "Skip an input by leaving it empty (e.g., ',100,').\n> ";
}

Extent3D parse_Extent3D(const std::string& input) {
    Extent3D result = {0, 0, 0};
    if (input.empty()) return result;

    std::stringstream ss(input);
    std::string segment;
    uint32_t* targets[] = {&result.width, &result.height, &result.depth};

    for (int i = 0; i < 3; ++i) {
        if (std::getline(ss, segment, ',')) {
            segment.erase(0, segment.find_first_not_of(" \t"));
            size_t last = segment.find_last_not_of(" \t");
            if (last != std::string::npos) segment.erase(last + 1);

            if (!segment.empty()) {
                try {
                    *targets[i] = static_cast<uint32_t>(std::stoul(segment));
                } catch (...) {
                    *targets[i] = 0;
                }
            } else {
                *targets[i] = 0;
            }
        } else {
            *targets[i] = 0;
        }
    }
    return result;
}


std::string default_input_Extent3D(uint32_t default_width, uint32_t default_height, uint32_t default_depth) {
    return std::to_string(default_width) + "," + std::to_string(default_height) + "," + std::to_string(default_depth);
}

std::string prompt_Origin3D(uint32_t default_x, uint32_t default_y, uint32_t default_z) {
    return "Enter x, y, z (comma separated).\n"
           "Defaults: " + std::to_string(default_x) + ", " +
           std::to_string(default_y) + ", " + std::to_string(default_z) + "\n"
           "Skip an input by leaving it empty (e.g., ',,10').\n> ";
}

Origin3D parse_Origin3D(const std::string& input) {
    Origin3D result = {0, 0, 0};
    if (input.empty()) return result;

    std::stringstream ss(input);
    std::string segment;
    uint32_t* targets[] = {&result.x, &result.y, &result.z};

    for (int i = 0; i < 3; ++i) {
        if (std::getline(ss, segment, ',')) {
            segment.erase(0, segment.find_first_not_of(" \t"));
            size_t last = segment.find_last_not_of(" \t");
            if (last != std::string::npos) segment.erase(last + 1);

            if (!segment.empty()) {
                try {
                    *targets[i] = static_cast<uint32_t>(std::stoul(segment));
                } catch (...) {
                    *targets[i] = 0;
                }
            } else {
                *targets[i] = 0;
            }
        } else {
            *targets[i] = 0;
        }
    }
    return result;
}


std::string default_input_Origin3D(uint32_t default_x, uint32_t default_y, uint32_t default_z) {
    return std::to_string(default_x) + "," + std::to_string(default_y) + "," + std::to_string(default_z);
}
}