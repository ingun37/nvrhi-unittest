//
// Created by Ingun Jon on 1/21/26.
//

#ifndef NVRHI_UNIT_TEST_RENDERPASS_H
#define NVRHI_UNIT_TEST_RENDERPASS_H

#include "include/scenario/Step.h"

struct RenderPassColorClearDraw : public Step {
    RenderPassColorClearDraw() = delete;

    explicit RenderPassColorClearDraw(const Context& ctx)
        : Step(ctx, "RenderPassColorClearDraw", "", "") {
    }

    StepFuture run(std::string) override;
};


#endif //NVRHI_UNIT_TEST_RENDERPASS_H