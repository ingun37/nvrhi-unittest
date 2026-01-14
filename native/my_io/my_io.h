//
// Created by Ingun Jon on 12/24/25.
//

#ifndef NVRHI_UNIT_TEST_MY_IO_H
#define NVRHI_UNIT_TEST_MY_IO_H
#include <string>
#include <functional>
#include <utility>

namespace my_io {
struct Rect {
    uint32_t x, y, width, height;
};
struct Box {
    uint32_t x, y, z, width, height, depth;
};

struct Extent3D {
    uint32_t width, height, depth;
};

struct Origin3D {
    uint32_t x, y, z;
};

std::string prompt_rect(uint32_t default_x, uint32_t default_y, uint32_t default_width, uint32_t default_height);

Rect parse_rect(const std::string& input);

std::string default_input_rect(uint32_t default_x, uint32_t default_y, uint32_t default_width, uint32_t default_height);

std::string prompt_box(uint32_t default_x,
                       uint32_t default_y,
                       uint32_t default_z,
                       uint32_t default_width,
                       uint32_t default_height,
                       uint32_t default_depth);

Box parse_box(const std::string& input);


std::string default_input_box(uint32_t default_x,
                              uint32_t default_y,
                              uint32_t default_z,
                              uint32_t default_width,
                              uint32_t default_height,
                              uint32_t default_depth);

std::string prompt_Extent3D(uint32_t default_width, uint32_t default_height, uint32_t default_depth);

Extent3D parse_Extent3D(const std::string& input);


std::string default_input_Extent3D(uint32_t default_width, uint32_t default_height, uint32_t default_depth);

std::string prompt_Origin3D(uint32_t default_x, uint32_t default_y, uint32_t default_z);

Origin3D parse_Origin3D(const std::string& input);

std::string default_input_Origin3D(uint32_t default_x, uint32_t default_y, uint32_t default_z);
}

#endif //NVRHI_UNIT_TEST_MY_IO_H