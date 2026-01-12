//
// Created by Ingun Jon on 12/23/25.
//

#include <scenario/scenario.h>
#include <iostream>
#include <string>
#include <sstream>
#include "Copy2DTexture.h"
#include "Copy3D.h"
#include "Map2DStaging.h"
#include "Map3DStaging.h"
#include "Map3DStagingMipMap.h"
#include "Shader.h"

AppPtr app_factory(Scenario scenario, Context&& webGpu) {
    switch (scenario) {
        case Scenario::COPY_2D_STAGING_TO_TEXTURE:
            return std::make_unique<Copy2D>(webGpu);
        case Scenario::COPY_3D:
            return std::make_unique<Copy3D>(webGpu);
        case Scenario::MAP_2D_STAGING:
            return std::make_unique<Map2DStaging>(webGpu);
        case Scenario::MAP_3D_STAGING:
            return std::make_unique<Map3DStaging>(webGpu);
        case Scenario::MAP_3D_STAGING_MIPMAP:
            return std::make_unique<Map3DStagingMipMap>(webGpu);
        case Scenario::SHADER:
            return std::make_unique<Shader>(webGpu);
        default:
            throw std::runtime_error("Invalid scenario");
    }
}

void run_app(Context&& webGpu) {
    std::string input;

    while (true) {
        std::cout << "\nAvailable Scenarios:" << std::endl;
        std::cout << "1: Copy 2D Staging to Texture" << std::endl;
        std::cout << "2: Shader" << std::endl;
        std::cout << "3: Copy 3D" << std::endl;
        std::cout << "4: Map 2D Staging" << std::endl;
        std::cout << "5: Map 3D Staging" << std::endl;
        std::cout << "6: Map 3D Staging Mipmap" << std::endl;
        std::cout << "Type 'exit' to quit." << std::endl;
        std::cout << "Select a scenario: ";

        std::getline(std::cin, input);
        if (input == "exit") break;

        Scenario scenario;
        if (input == "1") scenario = Scenario::COPY_2D_STAGING_TO_TEXTURE;
        else if (input == "2") scenario = Scenario::SHADER;
        else if (input == "3") scenario = Scenario::COPY_3D;
        else if (input == "4") scenario = Scenario::MAP_2D_STAGING;
        else if (input == "5") scenario = Scenario::MAP_3D_STAGING;
        else if (input == "6") scenario = Scenario::MAP_3D_STAGING_MIPMAP;
        else {
            std::cout << "Invalid selection. Try again." << std::endl;
            continue;
        }

        try {
            // Note: We use a copy or shared context if we want to keep the loop running
            // since app_factory currently takes Context&& (moves it).
            // If App needs ownership, you might need to re-initialize the context or pass a reference.
            AppPtr app = app_factory(scenario, std::move(webGpu));

            while (app) {
                std::cout << "\nRunning: " << app->title << std::endl;
                std::cout << "Press Enter to step, or type 'back' to choose another scenario: ";
                std::getline(std::cin, input);

                if (input == "back") break;
                if (input == "exit") return;

                app = app->run();
            }
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            break; // Exit or handle re-initialization of webGpu context
        }
    }
}