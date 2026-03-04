//
// Created by Ingun Jon on 3/4/26.
//

#ifndef NVRHI_UNIT_TEST_STENCIL_H
#define NVRHI_UNIT_TEST_STENCIL_H

#include "include/scenario/Step.h"

struct Stencil : Step {
    Stencil() = delete;

    explicit Stencil(const Context& ctx)
        : Step(ctx, "Stencil", "", "") {
    }

    StepFuture run(std::string) override;
};


#endif //NVRHI_UNIT_TEST_STENCIL_H