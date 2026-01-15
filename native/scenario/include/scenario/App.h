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
    std::string title;
    std::string prompt;
    std::string defaultInput;

    App(const Context& webGpu, std::string title, std::string prompt, std::string defaultInput)
        : context(webGpu),
          title(std::move(title)),
          prompt(std::move(prompt)),
          defaultInput(std::move(defaultInput)) {
    }

    App() = delete;


    virtual std::unique_ptr<App> run(std::string input) =0;
};

using AppPtr = std::unique_ptr<App>;

#endif //NVRHI_UNIT_TEST_APP_H