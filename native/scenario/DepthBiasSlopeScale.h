//
// Created by Ingun Jon on 2/28/26.
//

#ifndef NVRHI_UNIT_TEST_DEPTHBIASSLOPESCALE_H
#define NVRHI_UNIT_TEST_DEPTHBIASSLOPESCALE_H
#include "include/scenario/Step.h"


struct DepthBiasSlopeScale : Step {
    DepthBiasSlopeScale() = delete;

    explicit DepthBiasSlopeScale(const Context& ctx)
        : Step(ctx, "DepthBiasSlopeScale", "", "") {
    }

    StepFuture run(std::string) override;
};


#endif //NVRHI_UNIT_TEST_DEPTHBIASSLOPESCALE_H