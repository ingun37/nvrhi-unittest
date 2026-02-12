//
// Created by Ingun Jon on 1/14/26.
//

#include "scenario/scenario.h"

#include <iostream>

#include "include/scenario/ChooseScenario.h"

void run_app(Context&& ctx) {
#if defined(EMSCRIPTEN)
    throw std::runtime_error("run_app breaks in Emscripten. Use the scenario runner for emscripten in /web/.");
#endif
    StepFuture app_p = create_step_immediately<ChooseScenario>(ctx);
    while (true) {
        auto app = app_p.get();
        if (app == nullptr) {
            std::cout << "NULL is (lazily) returned as the next app. Exiting..." << std::endl;
            break;
        }
        std::string input;
        if (app->prompt.empty()) {
            std::cout << "Running " << app->title << " ... This app doesn't have propmt. Skipping input ..." <<
                std::endl;
        } else {
            std::cout << "Running " << app->title << " ..." << std::endl;
            std::cout << app->prompt << std::endl;
            std::cout << "Default value: " << app->defaultInput << std::endl;
            std::cout << "> ";
            std::getline(std::cin, input);
        }
        StepFuture next_app_p = app->run(input.empty() ? app->defaultInput : input);
        std::cout << "Repeat (y/n)? default: n" << std::endl;
        std::cout << "> ";
        std::string repeat;
        std::getline(std::cin, repeat);
        if (repeat == "y") {
            app_p = create_step_immediately(std::move(app));
        } else {
            app_p = std::move(next_app_p);
        }
    }
}