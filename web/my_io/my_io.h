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
struct Box {
    uint32_t x, y, z, width, height, depth;
};

struct Extent3D {
    uint32_t width, height, depth;
};

struct Origin3D {
    uint32_t x, y, z;
};

Rect read_rect(uint32_t default_x, uint32_t default_y, uint32_t default_width, uint32_t default_height);

Box read_box(uint32_t default_x,
             uint32_t default_y,
             uint32_t default_z,
             uint32_t default_width,
             uint32_t default_height,
             uint32_t default_depth
    );
Extent3D read_Extent3D(uint32_t default_width, uint32_t default_height, uint32_t default_depth);

Origin3D read_Origin3D(uint32_t default_x, uint32_t default_y, uint32_t default_z);
}

#endif //NVRHI_UNIT_TEST_MY_IO_H