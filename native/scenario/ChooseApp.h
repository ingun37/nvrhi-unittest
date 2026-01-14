//
// Created by Ingun Jon on 1/14/26.
//

#ifndef NVRHI_UNIT_TEST_CHOOSEAPP_H
#define NVRHI_UNIT_TEST_CHOOSEAPP_H
#include "App.h"

constexpr std::string_view scenarios[] = {
    "MAP_3D_STAGING_MIPMAP",
    "MapStagingAsync"
};


static std::string foo() {
    std::string result = "Available Scenarios:\n";
    for (size_t i = 0; i < sizeof(scenarios) / sizeof(scenarios[0]); ++i) {
        result += std::to_string(i) + ". " + std::string(scenarios[i]) + "\n";
    }
    result += "Enter scenario number: ";
    return result;
}

struct ChooseApp : public App {
    std::unique_ptr<App> run(std::string input) override;

    explicit ChooseApp(const Context& ctx)
        : App(ctx, "Choose App", foo(), "0") {
    }
};

#endif //NVRHI_UNIT_TEST_CHOOSEAPP_H