//
// Created by Ingun Jon on 12/23/25.
//

#include "scenario.h"
#include <iostream>
#include <string>
#include <sstream>

void run_app(Context&& webGpu, Scenario scenario) {
    std::string input;

    AppPtr app = app_factory(scenario, std::move(webGpu));
    while (true) {
        std::cout << "Enter to:" << app->title << std::endl;
        std::getline(std::cin, input);
        if (input == "exit") break;
        app = std::move(app->run());
    }
}