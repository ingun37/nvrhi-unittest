//
// Created by Ingun Jon on 12/23/25.
//

#include <scenario/scenario.h>
#include <iostream>
#include <string>
#include <sstream>
#include "Triangle.h"
#include "Copy2DTexture.h"
#include "Copy3D.h"

AppPtr app_factory(Scenario scenario, Context&& webGpu) {
    switch (scenario) {
        case Scenario::COPY_2D_STAGING_TO_TEXTURE:
            return std::make_unique<Copy2D>(webGpu);
        case Scenario::TRIANGLE:
            return std::make_unique<Triangle>(std::move(webGpu));
        case Scenario::COPY_3D:
            return std::make_unique<Copy3D>(webGpu);
        default:
            throw std::runtime_error("Invalid scenario");
    }
}

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