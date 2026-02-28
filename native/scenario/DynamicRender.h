//
// Created by Ingun Jon on 2/28/26.
//

#ifndef NVRHI_UNIT_TEST_DYNAMICRENDER_H
#define NVRHI_UNIT_TEST_DYNAMICRENDER_H
#include "include/scenario/Step.h"


struct DynamicRender : Step {
    DynamicRender() = delete;

    DynamicRender(const Context& ctx)
        : Step(ctx, "Dynamic Render", "", "") {
    }

    StepFuture run(std::string) override;
};


#endif //NVRHI_UNIT_TEST_DYNAMICRENDER_H