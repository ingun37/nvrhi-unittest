//
// Created by Ingun Jon on 1/14/26.
//
#include <memory>
#include "ChooseApp.h"
#include "Map3DStagingMipMap.h"
#include "MapStagingAsync.h"

AppPtr app_factory(int scenario, const Context& context) {
    if (scenario < 0 || scenario >= std::size(scenarios)) {
        throw std::runtime_error("Invalid scenario");
    }

    if (scenarios[scenario] == "MAP_3D_STAGING_MIPMAP") {
        return immediate_app(std::make_unique<Map3DStagingMipMap>(context));
    }
    if (scenarios[scenario] == "MapStagingAsync") {
        return immediate_app(std::make_unique<MapStagingAsync>(context));
    }

    throw std::runtime_error("Scenario not implemented in factory");
}

AppPtr ChooseApp::run(std::string input) {
    int scenarioNum = std::stoi(input); // Subtract 1 because UI list is 1-indexed
    return app_factory(scenarioNum,
                       context);
}