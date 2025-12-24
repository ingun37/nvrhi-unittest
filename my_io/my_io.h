//
// Created by Ingun Jon on 12/24/25.
//

#ifndef NVRHI_UNIT_TEST_MY_IO_H
#define NVRHI_UNIT_TEST_MY_IO_H
#include <cstdint>

namespace my_io {
struct Rect {
    uint32_t x, y, width, height;
};

Rect read_rect(uint32_t default_x, uint32_t default_y, uint32_t default_width, uint32_t default_height);
}
#endif //NVRHI_UNIT_TEST_MY_IO_H