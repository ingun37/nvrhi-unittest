//
// Created by Ingun Jon on 2/26/26.
//

#ifndef NVRHI_UNIT_TEST_MOVABLETRIANGLE_H
#define NVRHI_UNIT_TEST_MOVABLETRIANGLE_H
#include "include/scenario/Step.h"


struct DepthBias : Step {
    DepthBias() = delete;

    explicit DepthBias(const Context& ctx)
        : Step(ctx, "Movable Triangle", "", "") {
    }

    StepFuture run(std::string) override;
};


#endif //NVRHI_UNIT_TEST_MOVABLETRIANGLE_H