//
// Created by Ingun Jon on 12/23/25.
//

#ifndef NVRHI_UNIT_TEST_APP_H
#define NVRHI_UNIT_TEST_APP_H
#include <memory>
#include <string>
#include <utility>
#include <scenario/Context.h>


struct App {
    virtual ~App() = default;

    const Context& context;
    const std::string title;

    App(const Context& webGpu, std::string title)
        : context(webGpu),
          title(std::move(title)) {
    }

    virtual std::unique_ptr<App> run() =0;
};

using AppPtr = std::unique_ptr<App>;

#endif //NVRHI_UNIT_TEST_APP_H