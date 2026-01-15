//
// Created by Ingun Jon on 1/14/26.
//

#ifndef NVRHI_UNIT_TEST_TERMINATION_H
#define NVRHI_UNIT_TEST_TERMINATION_H
#include "include/scenario/App.h"

struct Termination : public App {
    Termination() = delete;

    Termination(const Context& ctx)
        : App(ctx, "Termination", "", "") {
    }

    AppPtr run(std::string input) override {
        return immediate_app(std::make_unique<Termination>(context));
    }
};


#endif //NVRHI_UNIT_TEST_TERMINATION_H