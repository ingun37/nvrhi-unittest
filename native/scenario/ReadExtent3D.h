//
// Created by Ingun Jon on 1/14/26.
//

#ifndef NVRHI_UNIT_TEST_READEXTENT3D_H
#define NVRHI_UNIT_TEST_READEXTENT3D_H


#include "include/scenario/App.h"
#include <my_io.h>
#include <functional>
#include <utility>

struct ReadExtent3D : public App {
    std::function<AppPtr(my_io::Extent3D e)> next;

    ReadExtent3D() = delete;

    explicit ReadExtent3D(
        const Context& ctx,
        std::function<AppPtr(my_io::Extent3D e)> next,
        uint32_t default_width,
        uint32_t default_height,
        uint32_t default_depth
        )
        : App(ctx,
              "ReadExtent3D",
              my_io::prompt_Extent3D(default_width, default_height, default_depth),
              my_io::default_input_Extent3D(default_width, default_height, default_depth)
              ),
          next(std::move(next)) {
    };

    AppPtr run(std::string input) override;
};


#endif //NVRHI_UNIT_TEST_READEXTENT3D_H