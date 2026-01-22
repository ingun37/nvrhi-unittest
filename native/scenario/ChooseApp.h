//
// Created by Ingun Jon on 1/14/26.
//

#ifndef NVRHI_UNIT_TEST_CHOOSEAPP_H
#define NVRHI_UNIT_TEST_CHOOSEAPP_H
#include "include/scenario/App.h"


std::string getPrompt();
struct ChooseApp : public App {
    AppPtr run(std::string input) override;

    explicit ChooseApp(const Context& ctx)
        : App(ctx, "Choose App", getPrompt(), "0") {
    }
};

#endif //NVRHI_UNIT_TEST_CHOOSEAPP_H