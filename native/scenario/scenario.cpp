//
// Created by Ingun Jon on 1/14/26.
//

#include "scenario/scenario.h"

#include <iostream>

#include "ChooseApp.h"

void run_app(Context&& ctx) {
    AppPtr app = std::make_unique<ChooseApp>(ctx);
    while (app != nullptr) {
        std::cout << app->title << std::endl;
        std::cout << app->prompt << std::endl;
        std::cout << "Default value: " << app->defaultInput << std::endl;
        std::cout << "> ";
        std::string input;
        std::getline(std::cin, input);
        app = app->run(input.empty() ? app->defaultInput : input);
    }
}