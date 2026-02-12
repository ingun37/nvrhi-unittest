//
// Created by Ingun Jon on 1/14/26.
//

#ifndef NVRHI_UNIT_TEST_CHOOSEAPP_H
#define NVRHI_UNIT_TEST_CHOOSEAPP_H
#include "include/scenario/Step.h"


std::string getPrompt();

struct ChooseScenario : public Step {
    StepFuture run(std::string input) override;

    explicit ChooseScenario(const Context& ctx)
        : Step(ctx, "Choose Scenario", getPrompt(), "0") {
    }
};

#endif //NVRHI_UNIT_TEST_CHOOSEAPP_H