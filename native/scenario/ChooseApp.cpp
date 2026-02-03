//
// Created by Ingun Jon on 1/14/26.
//
#include <memory>
#include "ChooseApp.h"
#include "Map3DStagingMipMap.h"
#include "MapStagingAsync.h"
#include "RenderPassColorClearDraw.h"
#include "RenderPassDepthOnly.h"
#include "ConstantBuffer.h"

template <typename... Apps>
struct AppRegistry {
    static AppPtr create(int index, const Context& ctx) {
        int i = 0;
        AppPtr result;
        // Use a fold expression with a comma operator to avoid the ternary operator's copy issue
        (([&] {
            if (i++ == index) {
                result = create_app_immediately<Apps>(ctx);
            }
        }()), ...);

        return result;
    }


    static std::string getPrompt() {
        std::string result = "Available Scenarios:\n";
        int i = 0;
        const char* names[] = {typeid(Apps).name()...};

        for (int j = 0; j < sizeof...(Apps); ++j) {
            result += std::to_string(j) + ". " + names[j] + "\n";
        }
        result += "Enter scenario number: ";
        return result;
    }

    static size_t count() { return sizeof...(Apps); }
};

using MyScenarios = AppRegistry<
    Map3DStagingMipMap,
    MapStagingAsync,
    RenderPassColorClearDraw,
    RenderPassDepthOnly,
    ConstantBuffer
>;

std::string getPrompt() {
    return MyScenarios::getPrompt();
}

AppPtr ChooseApp::run(std::string input) {
    int scenarioNum = std::stoi(input);
    if (scenarioNum < 0 || scenarioNum >= MyScenarios::count()) {
        throw std::runtime_error("Invalid scenario");
    }
    return MyScenarios::create(scenarioNum, context);
}