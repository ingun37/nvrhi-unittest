//
// Created by Ingun Jon on 2/26/26.
//

#ifndef NVRHI_UNIT_TEST_CONSTANTBLEND_H
#define NVRHI_UNIT_TEST_CONSTANTBLEND_H

#include "include/scenario/Step.h"

struct ConstantBlend : public Step {
    ConstantBlend() = delete;

    explicit ConstantBlend(const Context& ctx)
        : Step(ctx, "ConstantBlend", "", "") {
    }

    StepFuture run(std::string) override;
};


#endif //NVRHI_UNIT_TEST_CONSTANTBLEND_H