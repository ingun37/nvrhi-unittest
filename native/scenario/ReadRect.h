//
// Created by Ingun Jon on 1/14/26.
//

#ifndef NVRHI_UNIT_TEST_READRECT_H
#define NVRHI_UNIT_TEST_READRECT_H
#include "App.h"
#include <my_io.h>
#include <functional>
#include <utility>

struct ReadRect : public App {
    std::function<AppPtr(my_io::Rect r)> next;

    ReadRect() = delete;

    explicit ReadRect(
        const Context& ctx,
        std::function<AppPtr(my_io::Rect r)> next,
        uint32_t default_x,
        uint32_t default_y,
        uint32_t default_width,
        uint32_t default_height
        )
        : App(ctx,
              "ReadRect",
              my_io::prompt_rect(default_x, default_y, default_width, default_height),
              my_io::default_input_rect(default_x, default_y, default_width, default_height)
              ),
          next(std::move(next)) {
    };

    AppPtr run(std::string) override;
};


#endif //NVRHI_UNIT_TEST_READRECT_H