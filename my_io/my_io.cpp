//
// Created by Ingun Jon on 12/24/25.
//

#include "my_io.h"
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

namespace my_io {
Rect read_rect(uint32_t default_x, uint32_t default_y, uint32_t default_width, uint32_t default_height) {
    std::cout << "Enter x, y, width, height (comma separated)." << std::endl;
    std::cout << "Defaults: " << default_x << ", " << default_y << ", " << default_width << ", " << default_height <<
        std::endl;
    std::cout << "Skip an input by leaving it empty (e.g., ',,100,')." << std::endl;
    std::cout << "> ";

    Rect result = {default_x, default_y, default_width, default_height};

    std::string input;
    if (!std::getline(std::cin, input) || input.empty()) {
        return result;
    }

    std::stringstream ss(input);
    std::string segment;
    uint32_t* targets[] = {&result.x, &result.y, &result.width, &result.height};
    uint32_t defaults[] = {default_x, default_y, default_width, default_height};

    for (int i = 0; i < 4; ++i) {
        if (std::getline(ss, segment, ',')) {
            // Trim whitespace
            segment.erase(0, segment.find_first_not_of(" \t"));
            size_t last = segment.find_last_not_of(" \t");
            if (last != std::string::npos) {
                segment.erase(last + 1);
            }

            if (!segment.empty()) {
                try {
                    *targets[i] = static_cast<uint32_t>(std::stoul(segment));
                } catch (...) {
                    *targets[i] = defaults[i];
                }
            } else {
                *targets[i] = defaults[i];
            }
        } else {
            *targets[i] = defaults[i];
        }
    }

    return result;
}
Box read_box(uint32_t default_x,
             uint32_t default_y,
             uint32_t default_z,
             uint32_t default_width,
             uint32_t default_height,
             uint32_t default_depth) {
    std::cout << "Enter x, y, z, width, height, depth (comma separated)." << std::endl;
    std::cout << "Defaults: " << default_x << ", " << default_y << ", " << default_z << ", "
        << default_width << ", " << default_height << ", " << default_depth << std::endl;
    std::cout << "Skip an input by leaving it empty (e.g., ',,,,100,')." << std::endl;
    std::cout << "> ";

    Box result = {default_x, default_y, default_z, default_width, default_height, default_depth};

    std::string input;
    if (!std::getline(std::cin, input) || input.empty()) {
        return result;
    }

    std::stringstream ss(input);
    std::string segment;
    uint32_t* targets[] = {&result.x, &result.y, &result.z, &result.width, &result.height, &result.depth};
    uint32_t defaults[] = {default_x, default_y, default_z, default_width, default_height, default_depth};

    for (int i = 0; i < 6; ++i) {
        if (std::getline(ss, segment, ',')) {
            // Trim whitespace
            segment.erase(0, segment.find_first_not_of(" \t"));
            size_t last = segment.find_last_not_of(" \t");
            if (last != std::string::npos) {
                segment.erase(last + 1);
            }

            if (!segment.empty()) {
                try {
                    *targets[i] = static_cast<uint32_t>(std::stoul(segment));
                } catch (...) {
                    *targets[i] = defaults[i];
                }
            } else {
                *targets[i] = defaults[i];
            }
        } else {
            *targets[i] = defaults[i];
        }
    }

    return result;
}
}