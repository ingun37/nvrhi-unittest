//
// Created by Ingun Jon on 1/14/26.
//

#ifndef NVRHI_UNIT_TEST_READBOX_H
#define NVRHI_UNIT_TEST_READBOX_H


#include "include/scenario/App.h"
#include <my_io.h>
#include <functional>
#include <utility>

struct ReadBox : public App {
    std::function<AppPtr(my_io::Box b)> next;

    ReadBox() = delete;

    explicit ReadBox(
        const Context& ctx,
        std::function<AppPtr(my_io::Box b)> next,
        uint32_t default_x,
        uint32_t default_y,
        uint32_t default_z,
        uint32_t default_width,
        uint32_t default_height,
        uint32_t default_depth
        )
        : App(ctx,
              "ReadBox",
              my_io::prompt_box(default_x, default_y, default_z, default_width, default_height, default_depth),
              my_io::default_input_box(default_x, default_y, default_z, default_width, default_height, default_depth)
              ),
          next(std::move(next)) {
    };

    AppPtr run(std::string input) override;
};


#endif //NVRHI_UNIT_TEST_READBOX_H