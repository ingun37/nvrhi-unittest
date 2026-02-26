//
// Created by Ingun Jon on 1/21/26.
//

#ifndef NVRHI_UNIT_TEST_RENDERPASSDEPTHONLY_H
#define NVRHI_UNIT_TEST_RENDERPASSDEPTHONLY_H
#include "include/scenario/Step.h"


struct RenderPassDepthOnly : public Step {
    RenderPassDepthOnly() = delete;

    explicit RenderPassDepthOnly(const Context& ctx)
        : Step(ctx, "RenderPassDepthOnly", "", "") {
    }

    StepFuture run(std::string) override;
};


#endif //NVRHI_UNIT_TEST_RENDERPASSDEPTHONLY_H