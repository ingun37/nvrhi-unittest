//
// Created by Ingun Jon on 1/14/26.
//

#ifndef NVRHI_UNIT_TEST_READORIGIN3D_H
#define NVRHI_UNIT_TEST_READORIGIN3D_H


#include "include/scenario/App.h"
#include <my_io.h>
#include <functional>
#include <utility>

struct ReadOrigin3D : public App {
    std::function<AppPtr(my_io::Origin3D o)> next;

    ReadOrigin3D() = delete;

    explicit ReadOrigin3D(
        const Context& ctx,
        std::function<AppPtr(my_io::Origin3D o)> next,
        uint32_t default_x,
        uint32_t default_y,
        uint32_t default_z
        )
        : App(ctx,
              "ReadOrigin3D",
              my_io::prompt_Origin3D(default_x, default_y, default_z),
              my_io::default_input_Origin3D(default_x, default_y, default_z)
              ),
          next(std::move(next)) {
    };

    AppPtr run(std::string input) override;
};

#endif //NVRHI_UNIT_TEST_READORIGIN3D_H