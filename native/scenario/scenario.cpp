//
// Created by Ingun Jon on 1/14/26.
//

#include "scenario/scenario.h"

#include <iostream>

#include "ChooseApp.h"

void run_app(Context&& ctx) {
#if defined(EMSCRIPTEN)
    throw std::runtime_error("run_app breaks in Emscripten. Use the scenario runner for emscripten in /web/.");
#endif
    AppPtr app_p = create_app_immediately<ChooseApp>(ctx);
    while (app_p != nullptr) {
        auto app = app_p->get_future().get();
        if (app == nullptr) {
            std::cout << "NULL is (lazily) returned as the next app. Exiting..." << std::endl;
            break;
        }
        std::cout << app->title << std::endl;
        std::cout << app->prompt << std::endl;
        std::cout << "Default value: " << app->defaultInput << std::endl;
        std::cout << "> ";
        std::string input;
        std::getline(std::cin, input);
        app_p = app->run(input.empty() ? app->defaultInput : input);

        if (app_p == nullptr) {
            std::cout << "NULL is returned as the next app. Exiting..." << std::endl;
            break;
        }
    }
}