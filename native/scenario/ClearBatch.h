//
// Created by Ingun Jon on 3/3/26.
//

#ifndef NVRHI_UNIT_TEST_CLEARBATCH_H
#define NVRHI_UNIT_TEST_CLEARBATCH_H
#include "include/scenario/Step.h"


struct ClearBatch : Step {
    ClearBatch() = delete;

    explicit ClearBatch(const Context& ctx)
        : Step(ctx, "Clear Batch", "", "") {
    }

    StepFuture run(std::string) override;
};


#endif //NVRHI_UNIT_TEST_CLEARBATCH_H